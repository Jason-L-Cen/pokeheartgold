# SPDX-License-Identifier: GPL-3.0-or-later
"""Compile recovered lobby namespace views with their original C symbols and one shared state."""
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def masked(text):
    return re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda m:re.sub(r'[^\n]',' ',m[0]),text,flags=re.S)

def native_views(text):
    clean=masked(text);edits=[];aliases=[];previous=0
    for match in re.finditer(r'^namespace\s+(\w+)\s*\{',clean,re.M):
        if match.start()<previous: continue
        opening=match.end()-1;end=opening+1;depth=1
        while depth:
            depth+=(clean[end]=='{')-(clean[end]=='}');end+=1
        previous=end;name=match[1];body=text[match.start():end]
        declarations=[];names={};plain=masked(body);levels=[];depth=0
        for char in plain:levels.append(depth);depth+=(char=='{')-(char=='}')
        blocks=[]
        for block in re.finditer(r'\bextern\s+"C"\s*\{',body):
            if not plain[block.start():block.start()+6].strip():continue
            block_open=block.end()-1;stop=block_open+1;depth=1
            while depth:depth+=(plain[stop]=='{')-(plain[stop]=='}');stop+=1
            blocks.append((block_open,stop,levels[block_open]+1))
        # shortcut: reconstructed C declarations, one declarator per statement; use a C++ AST for new declarator forms.
        for decl in re.finditer(r'(?m)^[ \t]*([^#;\n{}]+(?:\n[^#;{}]+)*?)([;{])',plain):
            start=decl.start(1);raw=body[start:decl.end(1)].strip()
            direct=re.match(r'extern\s+"C"\s+',raw)
            in_block=any(a<start<b and levels[start]==level for a,b,level in blocks)
            if not direct and not in_block:continue
            signature=re.sub(r'^extern\s+(?:"C"\s+)?','',raw).strip()
            if re.match(r'(?:static|inline|typedef|class|namespace)\b',signature):continue
            if re.match(r'(?:struct|enum)\b',signature) and '(' not in signature:continue
            nesting=0
            for char in masked(signature):
                nesting+=(char in '([<')-(char in ')]>')
                if char==',' and not nesting:raise ValueError('Separate combined C declarations: '+signature)
            fn=re.search(r'\b(\w+)\s*\(',signature)
            if fn:
                symbol=fn[1]
                if symbol in ('void','int','char'):raise ValueError('Unsupported C declaration: '+signature)
            else:
                var=re.search(r'\b(\w+)\s*(?:\[[^\]]*\])?$',signature)
                if not var:raise ValueError('Unsupported C declaration: '+signature)
                symbol=var[1]
            if '=' in signature:raise ValueError('Initialized C declaration in namespace: '+signature)
            alias='hg_view_'+name+'_'+symbol;names[symbol]=alias
            declarations.append((decl,symbol,signature))
        body_edits=[]
        for decl,symbol,signature in declarations:
            if decl[2]==';':body_edits.append((decl.end()-1,decl.end()-1,' __asm__("'+symbol+'")'))
            else:
                signature=re.sub(r'\b'+symbol+r'\b',names[symbol],signature)
                body_edits.append((decl.start(),decl.start(),'extern "C" '+signature+' __asm__("'+symbol+'");\n'))
        for token in re.finditer(r'\b\w+\b',masked(body)):
            if token.start()>opening-match.start() and token[0] in names:body_edits.append((token.start(),token.end(),names[token[0]]))
        for start,stop,replacement in sorted(body_edits,reverse=True):body=body[:start]+replacement+body[stop:]
        edits.append((match.start(),end,body));aliases.extend(dict(namespace=name,symbol=symbol,alias=alias) for symbol,alias in names.items())
    for start,end,body in reversed(edits):text=text[:start]+body+text[end:]
    for row in aliases:
        for m in reversed(list(re.finditer(r'\b'+row['namespace']+r'\s*::\s*'+row['symbol']+r'\b',masked(text)))):
            text=text[:m.start()]+row['namespace']+'::'+row['alias']+text[m.end():]
    for m in reversed(list(re.finditer(r'\b(?:class|struct)\s+\w+\s*\{\s*\}',masked(text)))):
        text=text[:m.end()-1]+'u8 hgAbiCarrier;'+text[m.end()-1:]
    return text,aliases

def prepare(output):
    source=ROOT/'src/lobby/ppw_lobby.cpp'
    code,aliases=native_views(source.read_text())
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    prepared=output/'ppw-native.cpp'
    prepared.write_text('#line 1 '+json.dumps(str(source))+'\n'+code)
    (output/'aliases.json').write_text(json.dumps(aliases,indent=2)+'\n')
    return [prepared]


def build(output, sdk, flags, target, overlay_key=None):
    output = Path(output); target = Path(target)
    objects, diagnostics, states = [], [], []
    for source in prepare(output):
        obj = source.with_suffix('.o')
        if overlay_key is not None:
            from prepare_overlay_state import compile_unit
            code,detail,state=compile_unit(sdk/'bin/clang++',flags,source,obj,overlay_key+'/'+source.name,ROOT)
            states.append(state)
        else:
            result = subprocess.run([str(sdk / 'bin/clang++'),*map(str,flags),'-c',str(source),'-o',str(obj)],cwd=ROOT,capture_output=True,text=True)
            code,detail=result.returncode,result.stdout+result.stderr
        source.with_suffix('.log').write_text(detail)
        diagnostics.append(detail)
        if code:
            obj.unlink(missing_ok=True); target.unlink(missing_ok=True)
            return code,''.join(diagnostics)
        objects.append(obj)
    if overlay_key is not None: (output/'overlay-state.json').write_text(json.dumps(states,indent=2)+'\n')
    result = subprocess.run([str(sdk / 'bin/wasm-ld'),'--relocatable','--fatal-warnings',*map(str,objects),'-o',str(target)],
                            cwd=ROOT,capture_output=True,text=True)
    diagnostics.append(result.stdout+result.stderr)
    (output / 'link.log').write_text(result.stdout+result.stderr)
    if result.returncode: target.unlink(missing_ok=True)
    else:
        archive = target.with_suffix('.a'); archive.unlink(missing_ok=True)
        result = subprocess.run([str(sdk / 'bin/llvm-ar'),'rcs',str(archive),*map(str,objects)],
                                cwd=ROOT,capture_output=True,text=True)
        diagnostics.append(result.stdout+result.stderr)
    return result.returncode,''.join(diagnostics)


if __name__ == '__main__':
    print(f'Prepared {len(prepare(ROOT / "ios/build-original-abi32/lobby-parts"))} native lobby translation unit.')
