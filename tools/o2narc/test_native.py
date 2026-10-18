"""Exercise LLVM ELF shared strings and malformed-input failures without a test framework."""
from pathlib import Path
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent
subprocess.run(['make','-s','-C',str(ROOT)],check=True)


def execute(args):
    return subprocess.run([str(arg) for arg in args],capture_output=True,text=True)


def members(path):
    data = path.read_bytes(); assert data[:4] == b'NARC'
    fat = struct.unpack_from('<H',data,12)[0]
    fat_size,count = struct.unpack_from('<IH',data,fat+4)
    names = fat+fat_size; images = names+struct.unpack_from('<I',data,names+4)[0]+8
    return [data[images+a:images+b] for a,b in
            (struct.unpack_from('<II',data,fat+12+i*8) for i in range(count))]


with tempfile.TemporaryDirectory() as temporary:
    folder = Path(temporary); source = folder/'data.c'; obj = folder/'data.o'; target = folder/'sample.narc'
    source.write_text('const unsigned first[] = {1}; const unsigned second[] = {2};\n')
    result = execute(['xcrun','clang','--target=armv5te-none-eabi','-c',source,'-o',obj]); assert result.returncode == 0,result.stderr
    elf = obj.read_bytes(); offset=struct.unpack_from('<I',elf,32)[0]; count,strings=struct.unpack_from('<HH',elf,48)
    symbol_section=next(offset+i*40 for i in range(count) if struct.unpack_from('<I',elf,offset+i*40+4)[0] == 2)
    assert struct.unpack_from('<I',elf,symbol_section+24)[0] == strings # LLVM shares section/symbol strings.
    result = execute([ROOT/'o2narc',obj,target,'-N']); assert result.returncode == 0,result.stderr
    assert members(target) == [struct.pack('<I',1),struct.pack('<I',2)]
    assert 'NARC_sample_first = 0' in target.with_suffix('.naix').read_text()
    damaged = bytearray(elf); struct.pack_into('<I',damaged,symbol_section+24,0xFFFFFFFF)
    for payload in [b'\x7fELF', bytes(damaged)]:
        broken = folder/'broken.o'; broken.write_bytes(payload)
        result = execute([ROOT/'o2narc',broken,target,'-n'])
        assert result.returncode == 1 and 'ELF_ASSERT' in result.stderr,result.stderr
    source.write_text('const unsigned __data[] = {7,8,9}; unsigned __size = 4;\n')
    assert execute(['xcrun','clang','--target=armv5te-none-eabi','-c',source,'-o',obj]).returncode == 0
    result = execute([ROOT/'o2narc',obj,target,'-n']); assert result.returncode == 0,result.stderr
    assert members(target) == [struct.pack('<I',value) for value in (7,8,9)]
    source.write_text('const unsigned __data[] = {7}; unsigned __size __attribute__((section(".data"))) = 0;\n')
    assert execute(['xcrun','clang','--target=armv5te-none-eabi','-c',source,'-o',obj]).returncode == 0
    result = execute([ROOT/'o2narc',obj,target,'-n'])
    assert result.returncode == 1 and 'ELF_ASSERT' in result.stderr,result.stderr
print('PASS: native o2narc shared ELF strings, named/fixed-size data, truncated ELF, invalid links and zero record sizes.')
