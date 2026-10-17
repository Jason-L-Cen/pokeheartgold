// SPDX-License-Identifier: GPL-3.0-or-later
import MetalKit
import simd

private struct MeshInfo: Decodable {
    let model: String, file: String, texture: String, count: Int, transparent: Bool
}
private struct Mesh {
    let model: String, buffer: MTLBuffer, texture: MTLTexture, count: Int, transparent: Bool
}

final class WorldRenderer: NSObject, MTKViewDelegate {
    let view: MTKView
    let world: World
    var update: ((Double) -> Void)?
    var location = SIMD3<Float>(119.5, 1, 45.5)
    var followerLocation = SIMD3<Float>(120.5, 1, 45.5)
    var facing = 0, moving = false, running = false, follower: Int?
    private let resources: URL, device: MTLDevice, queue: MTLCommandQueue
    private var pipeline: MTLRenderPipelineState!, depth: MTLDepthStencilState!, sampler: MTLSamplerState!
    private var meshes: [Mesh] = [], textures: [String: MTLTexture] = [:]
    private var white: MTLTexture!
    private var lastTime = 0.0
    private var smoothLocation = SIMD3<Float>(119.5, 1, 45.5)

    init(view: MTKView, resources: URL, world: World) throws {
        guard let device = MTLCreateSystemDefaultDevice(), let queue = device.makeCommandQueue() else {
            throw NSError(domain: "Metal", code: 1, userInfo: [NSLocalizedDescriptionKey: "这台设备无法启动 Metal 渲染。"])
        }
        self.view = view; self.resources = resources; self.world = world; self.device = device; self.queue = queue
        super.init()
        view.device = device; view.colorPixelFormat = .bgra8Unorm; view.depthStencilPixelFormat = .depth32Float
        view.clearColor = MTLClearColorMake(0.25, 0.41, 0.29, 1)
        view.preferredFramesPerSecond = 60; view.isOpaque = true
        let shader = try device.makeLibrary(source: Self.shader, options: nil)
        let descriptor = MTLRenderPipelineDescriptor()
        descriptor.vertexFunction = shader.makeFunction(name: "worldVertex")
        descriptor.fragmentFunction = shader.makeFunction(name: "worldFragment")
        descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat
        descriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat
        descriptor.colorAttachments[0].isBlendingEnabled = true
        descriptor.colorAttachments[0].sourceRGBBlendFactor = .sourceAlpha
        descriptor.colorAttachments[0].destinationRGBBlendFactor = .oneMinusSourceAlpha
        descriptor.colorAttachments[0].sourceAlphaBlendFactor = .one
        descriptor.colorAttachments[0].destinationAlphaBlendFactor = .oneMinusSourceAlpha
        pipeline = try device.makeRenderPipelineState(descriptor: descriptor)
        let depthDescriptor = MTLDepthStencilDescriptor()
        depthDescriptor.depthCompareFunction = .lessEqual; depthDescriptor.isDepthWriteEnabled = true
        depth = device.makeDepthStencilState(descriptor: depthDescriptor)
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = .nearest; samplerDescriptor.magFilter = .nearest
        samplerDescriptor.sAddressMode = .repeat; samplerDescriptor.tAddressMode = .repeat
        sampler = device.makeSamplerState(descriptor: samplerDescriptor)
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba8Unorm, width: 1, height: 1, mipmapped: false)
        white = device.makeTexture(descriptor: textureDescriptor)
        var pixel: UInt32 = 0xffffffff
        white.replace(region: MTLRegionMake2D(0, 0, 1, 1), mipmapLevel: 0, withBytes: &pixel, bytesPerRow: 4)
        let info = try JSONDecoder().decode([MeshInfo].self, from: Data(contentsOf: resources.appendingPathComponent("meshes.json")))
        for mesh in info {
            let data = try Data(contentsOf: resources.appendingPathComponent(mesh.file))
            guard data.count == mesh.count * 36, let buffer = device.makeBuffer(bytes: [UInt8](data), length: data.count) else {
                throw NSError(domain: "Assets", code: 2, userInfo: [NSLocalizedDescriptionKey: "地图网格资源不完整。"])
            }
            meshes.append(Mesh(model: mesh.model, buffer: buffer, texture: try texture(mesh.texture), count: mesh.count, transparent: mesh.transparent))
        }
        for name in ["hero", "doctor", "follower-16", "follower-19", "follower-152", "follower-155", "follower-158", "follower-161", "follower-163"] {
            let count = name == "hero" ? 32 : name == "doctor" ? 16 : 8
            for frame in 0..<count { _ = try texture("\(name)-\(frame).png") }
        }
        view.delegate = self
    }
    private func texture(_ name: String) throws -> MTLTexture {
        if name.isEmpty { return white }
        if let cached = textures[name] { return cached }
        let result = try MTKTextureLoader(device: device).newTexture(URL: resources.appendingPathComponent(name), options: [.SRGB: false, .origin: MTKTextureLoader.Origin.topLeft])
        textures[name] = result
        return result
    }
    func resetCamera() { smoothLocation = location; lastTime = 0 }
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}
    func draw(in view: MTKView) {
        let now = CACurrentMediaTime(), delta = lastTime == 0 ? 0 : min(0.05, now - lastTime)
        lastTime = now
        update?(now)
        smoothLocation += (location - smoothLocation) * min(1, Float(delta) * 13)
        guard let pass = view.currentRenderPassDescriptor, let drawable = view.currentDrawable,
              let command = queue.makeCommandBuffer(), let encoder = command.makeRenderCommandEncoder(descriptor: pass) else { return }
        encoder.setRenderPipelineState(pipeline); encoder.setDepthStencilState(depth)
        encoder.setCullMode(.none); encoder.setFragmentSamplerState(sampler, index: 0)
        let aspect = Float(view.drawableSize.width / max(1, view.drawableSize.height))
        let target = smoothLocation + SIMD3<Float>(0, 0, -1)
        let eye = target + SIMD3<Float>(0, 19, 23)
        let vp = Self.orthographic(width: 19 * aspect, height: 19) * Self.lookAt(eye: eye, target: target)
        for transparent in [false, true] {
            for cell in world.cells {
                let center = SIMD3<Float>(Float(cell.x + 16), Float(cell.altitude) / 2, Float(cell.z + 16))
                guard abs(center.x - target.x) < 55, abs(center.z - target.z) < 50 else { continue }
                let root = Self.translation(center) * Self.scale(SIMD3<Float>(repeating: 1 / 16))
                for mesh in meshes where mesh.model == cell.model && mesh.transparent == transparent {
                    draw(mesh, matrix: vp * root, encoder: encoder)
                }
                for prop in cell.props {
                    let transform = root * Self.translation(SIMD3(prop.position)) * Self.rotationY(prop.rotation[1]) * Self.scale(SIMD3(prop.scale))
                    for mesh in meshes where mesh.model == prop.model && mesh.transparent == transparent {
                        draw(mesh, matrix: vp * transform, encoder: encoder)
                    }
                }
            }
        }
        billboard(at: SIMD3(108.5, world.elevation(108, 42), 42.5), height: 2.1, texture: textures["doctor-4.png"]!, matrix: vp, encoder: encoder)
        if let follower {
            let direction = [1, 0, 2, 3][facing]
            billboard(at: followerLocation, height: 1.8, texture: textures["follower-\(follower)-\(direction * 2 + (moving ? Int(now * 8) % 2 : 0)).png"]!, matrix: vp, encoder: encoder)
        }
        let frame = [4, 0, 8, 12][facing] + (running && moving ? 16 : 0) + (moving ? Int(now * 9) % 4 : 0)
        billboard(at: smoothLocation, height: 2.2, texture: textures["hero-\(frame).png"]!, matrix: vp, encoder: encoder)
        encoder.endEncoding(); command.present(drawable); command.commit()
    }
    private func draw(_ mesh: Mesh, matrix: simd_float4x4, encoder: MTLRenderCommandEncoder) {
        var transform = matrix
        encoder.setVertexBuffer(mesh.buffer, offset: 0, index: 0)
        encoder.setVertexBytes(&transform, length: MemoryLayout<simd_float4x4>.size, index: 1)
        encoder.setFragmentTexture(mesh.texture, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: mesh.count)
    }
    private func billboard(at position: SIMD3<Float>, height: Float, texture: MTLTexture, matrix: simd_float4x4, encoder: MTLRenderCommandEncoder) {
        let up = SIMD3<Float>(0, 0.771, -0.637) * height
        let right = SIMD3<Float>(height / 2, 0, 0)
        let a = position - right, b = position + right, c = b + up, d = a + up
        var vertices: [Float] = []
        for (p, uv) in [(a, SIMD2<Float>(0, 1)), (b, SIMD2<Float>(1, 1)), (c, SIMD2<Float>(1, 0)), (a, SIMD2<Float>(0, 1)), (c, SIMD2<Float>(1, 0)), (d, SIMD2<Float>(0, 0))] {
            vertices += [p.x, p.y + 0.05, p.z, uv.x, uv.y, 1, 1, 1, 1]
        }
        var transform = matrix
        vertices.withUnsafeBytes { encoder.setVertexBytes($0.baseAddress!, length: $0.count, index: 0) }
        encoder.setVertexBytes(&transform, length: MemoryLayout<simd_float4x4>.size, index: 1)
        encoder.setFragmentTexture(texture, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 6)
    }
    private static func translation(_ p: SIMD3<Float>) -> simd_float4x4 {
        var result = matrix_identity_float4x4; result.columns.3 = SIMD4(p, 1); return result
    }
    private static func scale(_ s: SIMD3<Float>) -> simd_float4x4 {
        simd_float4x4(diagonal: SIMD4(s, 1))
    }
    private static func rotationY(_ degrees: Float) -> simd_float4x4 {
        let angle = degrees * .pi / 180, c = cos(angle), s = sin(angle)
        return simd_float4x4(SIMD4(c, 0, -s, 0), SIMD4(0, 1, 0, 0), SIMD4(s, 0, c, 0), SIMD4(0, 0, 0, 1))
    }
    private static func lookAt(eye: SIMD3<Float>, target: SIMD3<Float>) -> simd_float4x4 {
        let z = normalize(eye - target), x = normalize(cross(SIMD3(0, 1, 0), z)), y = cross(z, x)
        return simd_float4x4(SIMD4(x.x, y.x, z.x, 0), SIMD4(x.y, y.y, z.y, 0), SIMD4(x.z, y.z, z.z, 0), SIMD4(-dot(x, eye), -dot(y, eye), -dot(z, eye), 1))
    }
    private static func orthographic(width: Float, height: Float) -> simd_float4x4 {
        simd_float4x4(SIMD4(2 / width, 0, 0, 0), SIMD4(0, 2 / height, 0, 0), SIMD4(0, 0, -1 / 100, 0), SIMD4(0, 0, 0, 1))
    }
    private static let shader = """
    #include <metal_stdlib>
    using namespace metal;
    struct Vertex { packed_float3 position; packed_float2 uv; packed_float4 color; };
    struct Out { float4 position [[position]]; float2 uv; float4 color; };
    vertex Out worldVertex(uint id [[vertex_id]], const device Vertex* vertices [[buffer(0)]], constant float4x4& matrix [[buffer(1)]]) {
        Vertex v = vertices[id]; Out o;
        o.position = matrix * float4(float3(v.position), 1);
        o.uv = float2(v.uv); o.color = float4(v.color); return o;
    }
    fragment float4 worldFragment(Out v [[stage_in]], texture2d<float> image [[texture(0)]], sampler nearest [[sampler(0)]]) {
        float4 color = image.sample(nearest, v.uv) * v.color;
        if (color.a < 0.15) discard_fragment();
        return color;
    }
    """
}

private extension SIMD3 where Scalar == Float {
    init(_ array: [Float]) { self.init(array[0], array[1], array[2]) }
}
