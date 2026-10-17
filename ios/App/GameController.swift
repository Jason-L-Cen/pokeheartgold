// SPDX-License-Identifier: GPL-3.0-or-later
import UIKit
import MetalKit

private let gold = UIColor(red: 1, green: 0.83, blue: 0.46, alpha: 1)
private let ink = UIColor(red: 0.04, green: 0.08, blue: 0.075, alpha: 1)

final class Joystick: UIControl {
    var vector = CGPoint.zero { didSet { setNeedsDisplay() } }
    var changed: ((CGPoint) -> Void)?
    override init(frame: CGRect) {
        super.init(frame: frame)
        backgroundColor = .clear; isMultipleTouchEnabled = false
        isAccessibilityElement = true; accessibilityLabel = "移动摇杆"
        accessibilityHint = "拖动移动，松手停止。也可使用自定操作向四个方向走动。"
    }
    required init?(coder: NSCoder) { fatalError() }
    override func draw(_ rect: CGRect) {
        let context = UIGraphicsGetCurrentContext()!, center = CGPoint(x: bounds.midX, y: bounds.midY)
        let circle = CGRect(x: center.x - 54, y: center.y - 54, width: 108, height: 108)
        context.setFillColor(ink.withAlphaComponent(0.42).cgColor); context.fillEllipse(in: circle)
        context.setStrokeColor(UIColor.white.withAlphaComponent(0.32).cgColor); context.setLineWidth(1.5); context.strokeEllipse(in: circle)
        let knob = CGRect(x: center.x - 24 + vector.x * 35, y: center.y - 24 + vector.y * 35, width: 48, height: 48)
        context.setFillColor(gold.withAlphaComponent(0.85).cgColor); context.fillEllipse(in: knob)
    }
    private func track(_ touch: UITouch) {
        let point = touch.location(in: self), dx = point.x - bounds.midX, dy = point.y - bounds.midY
        let distance = hypot(dx, dy), scale = max(48, distance)
        vector = distance < 11 ? .zero : CGPoint(x: dx / scale, y: dy / scale)
        changed?(vector)
    }
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) { if let touch = touches.first { track(touch) } }
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) { if let touch = touches.first { track(touch) } }
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) { reset() }
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) { reset() }
    func reset() { vector = .zero; changed?(.zero) }
}

final class MiniMap: UIView {
    let world: World
    var x = 119, z = 45
    init(_ world: World) { self.world = world; super.init(frame: .zero); backgroundColor = ink.withAlphaComponent(0.82); layer.cornerRadius = 14; clipsToBounds = true; isAccessibilityElement = true; accessibilityLabel = "若叶镇与 29 号道路地图" }
    required init?(coder: NSCoder) { fatalError() }
    override func draw(_ rect: CGRect) {
        let context = UIGraphicsGetCurrentContext()!, scale = bounds.width / 128
        for z in 32..<64 {
            for x in 0..<128 {
                let p = world.permission(x, z)
                let color = p & 0x8000 != 0 ? UIColor(red: 0.12, green: 0.25, blue: 0.18, alpha: 1) : p & 255 == 2 ? UIColor(red: 0.35, green: 0.63, blue: 0.32, alpha: 1) : UIColor(red: 0.70, green: 0.68, blue: 0.46, alpha: 1)
                context.setFillColor(color.cgColor); context.fill(CGRect(x: CGFloat(x) * scale, y: CGFloat(z - 32) * scale + 10, width: scale + 0.3, height: scale + 0.3))
            }
        }
        context.setFillColor(gold.cgColor); context.fillEllipse(in: CGRect(x: CGFloat(x) * scale - 3, y: CGFloat(z - 32) * scale + 7, width: 6, height: 6))
    }
}

final class GameController: UIViewController {
    private var game: Game!, renderer: WorldRenderer!, resources: URL!
    private let metalView = MTKView(), stick = Joystick(), titleLabel = UILabel(), objective = UILabel(), message = UILabel()
    private let main = UIButton(type: .system), bag = UIButton(type: .system), team = UIButton(type: .system), run = UIButton(type: .system), saveButton = UIButton(type: .system)
    private var miniMap: MiniMap!
    private var battleView: BattleView!
    private var running = false, nextStep = 0.0, nextSave = 0.0
    private var imageCache: [String: UIImage] = [:]
    private var messageUntil = 0.0

    override var supportedInterfaceOrientations: UIInterfaceOrientationMask { .landscape }
    override var preferredInterfaceOrientationForPresentation: UIInterfaceOrientation { .landscapeRight }
    override var prefersStatusBarHidden: Bool { true }
    override var prefersHomeIndicatorAutoHidden: Bool { true }
    override func viewDidLoad() {
        super.viewDidLoad(); view.backgroundColor = ink; view.isMultipleTouchEnabled = true
        do {
            resources = Bundle.main.resourceURL!.appendingPathComponent("NativeAssets")
            let storage = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask)[0].appendingPathComponent("HeartGoldNative/save-v1.json")
            game = try Game(resources: resources, saveURL: storage)
            battleView = try BattleView(resources: resources)
            view.addSubview(metalView); renderer = try WorldRenderer(view: metalView, resources: resources, world: game.world)
            renderer.update = { [weak self] now in self?.tick(now) }
            miniMap = MiniMap(game.world); view.addSubview(miniMap)
            for label in [titleLabel, objective, message] {
                label.textColor = .white; label.font = .preferredFont(forTextStyle: .subheadline)
                label.adjustsFontForContentSizeCategory = true; label.numberOfLines = 2
                view.addSubview(label)
            }
            titleLabel.font = .systemFont(ofSize: 20, weight: .bold)
            objective.font = .preferredFont(forTextStyle: .caption1); objective.textColor = gold
            for label in [titleLabel, objective] {
                label.layer.shadowColor = UIColor.black.cgColor; label.layer.shadowOpacity = 0.9
                label.layer.shadowRadius = 2; label.layer.shadowOffset = CGSize(width: 0, height: 1)
            }
            message.font = .preferredFont(forTextStyle: .subheadline); message.numberOfLines = 4
            message.backgroundColor = ink.withAlphaComponent(0.88); message.layer.cornerRadius = 12; message.clipsToBounds = true
            message.textAlignment = .center
            view.addSubview(stick)
            stick.changed = { [weak self] point in if point == .zero { self?.nextStep = 0 } }
            stick.accessibilityCustomActions = [("向上走", 0, -1), ("向下走", 0, 1), ("向左走", -1, 0), ("向右走", 1, 0)].map { title, dx, dz in
                UIAccessibilityCustomAction(name: title) { [weak self] _ in self?.step(dx, dz); return true }
            }
            style(main, title: "交互", symbol: "hand.tap", primary: true, action: #selector(interact))
            style(bag, title: "背包", symbol: "bag", action: #selector(openBag))
            style(team, title: "队伍", symbol: "person.2", action: #selector(openParty))
            style(run, title: "奔跑", symbol: "figure.run", action: #selector(toggleRun))
            style(saveButton, title: "存档", symbol: "square.and.arrow.down", action: #selector(manualSave))
            view.addSubview(battleView)
            battleView.action = { [weak self] action in self?.battleAction(action) }
            sync(); positionCamera(reset: true)
            if let error = game.loadError { show(error.localizedDescription, seconds: .infinity) }
            else { show(game.progress.battle?.result != nil ? "这场战斗已经结束，可以继续探索。" : game.isBattling ? "继续尚未结束的战斗。" : "左手摇杆移动，右手交互。先去左上方的研究所领取搭档。", seconds: 9) }
            NotificationCenter.default.addObserver(self, selector: #selector(suspendGame), name: UIApplication.willResignActiveNotification, object: nil)
            NotificationCenter.default.addObserver(self, selector: #selector(resumeGame), name: UIApplication.didBecomeActiveNotification, object: nil)
        } catch {
            titleLabel.text = error.localizedDescription; titleLabel.numberOfLines = 0; view.addSubview(titleLabel)
        }
    }
    private func style(_ button: UIButton, title: String, symbol: String, primary: Bool = false, action: Selector) {
        var config = UIButton.Configuration.plain()
        config.title = title; config.image = UIImage(systemName: symbol); config.imagePlacement = .top; config.imagePadding = 4
        config.cornerStyle = .capsule
        config.baseForegroundColor = primary ? ink : .white
        config.background.backgroundColor = primary ? gold : ink.withAlphaComponent(0.76)
        config.contentInsets = NSDirectionalEdgeInsets(top: 6, leading: 5, bottom: 6, trailing: 5)
        config.titleTextAttributesTransformer = UIConfigurationTextAttributesTransformer { original in
            var result = original; result.font = UIFont.preferredFont(forTextStyle: .caption1); return result
        }
        button.configuration = config; button.layer.borderColor = gold.withAlphaComponent(0.55).cgColor; button.layer.borderWidth = 1
        button.addTarget(self, action: action, for: .touchUpInside); button.accessibilityLabel = title
        view.addSubview(button)
    }
    private func label(_ button: UIButton, _ title: String, subtitle: String? = nil) {
        var config = button.configuration!; config.title = title; config.subtitle = subtitle; button.configuration = config
        button.accessibilityLabel = title; button.accessibilityValue = subtitle
    }
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews(); metalView.frame = view.bounds; battleView?.frame = view.bounds
        let safe = view.bounds.inset(by: view.safeAreaInsets), x = safe.minX + 16, y = safe.minY + 12, w = safe.width - 32
        miniMap?.frame = CGRect(x: x, y: y, width: 128, height: 52)
        titleLabel.frame = CGRect(x: x + 144, y: y, width: max(160, w - 270), height: 26)
        objective.frame = CGRect(x: x + 144, y: y + 28, width: max(160, w - 270), height: 34)
        stick.frame = CGRect(x: x, y: safe.maxY - 152, width: 136, height: 136)
        saveButton.frame = CGRect(x: safe.maxX - 74, y: y, width: 58, height: 58)
        let cx = safe.maxX - 74, cy = safe.maxY - 68
        main.frame = CGRect(x: cx - 46, y: cy - 46, width: 92, height: 92)
        bag.frame = CGRect(x: cx - 134, y: cy - 32, width: 64, height: 64)
        team.frame = CGRect(x: cx - 112, y: cy - 108, width: 64, height: 64)
        run.frame = CGRect(x: cx - 32, y: cy - 134, width: 64, height: 64)
        for button in [main, bag, team, run, saveButton] { button.layer.cornerRadius = button.bounds.width / 2; button.clipsToBounds = true }
        let messageWidth = min(470, max(220, w - 350))
        let messageHeight = min(112, ceil(message.sizeThatFits(CGSize(width: messageWidth - 20, height: 200)).height) + 20)
        message.frame = CGRect(x: safe.midX - messageWidth / 2 - 15, y: safe.maxY - messageHeight - 18, width: messageWidth, height: messageHeight)
    }
    private func tick(_ now: Double) {
        guard game != nil else { return }
        if !game.isBattling && presentedViewController == nil && stick.vector != .zero && now >= nextStep {
            nextStep = now + (running ? 0.11 : 0.22)
            let v = stick.vector
            if abs(v.x) > abs(v.y) { step(v.x > 0 ? 1 : -1, 0) }
            else { step(0, v.y > 0 ? 1 : -1) }
        }
        renderer.moving = !game.isBattling && stick.vector != .zero
        renderer.running = running
        if now > messageUntil && !game.isBattling { message.isHidden = true }
        if now > nextSave { nextSave = now + 15; persist() }
    }
    private func step(_ dx: Int, _ dz: Int) {
        let oldX = game.progress.x, oldZ = game.progress.z
        let hour = Calendar.current.component(.hour, from: Date()), time = hour < 4 || hour >= 20 ? "nite" : hour < 10 ? "morn" : "day"
        let messages = game.move(dx: dx, dz: dz, time: time)
        if game.progress.x != oldX || game.progress.z != oldZ {
            renderer.followerLocation = SIMD3(Float(oldX) + 0.5, game.world.elevation(oldX, oldZ), Float(oldZ) + 0.5)
        }
        positionCamera(); sync()
        if !messages.isEmpty { show(messages.joined(separator: "\n")) }
        if game.isBattling { stick.reset(); persist() }
    }
    private func positionCamera(reset: Bool = false) {
        renderer.location = SIMD3(Float(game.progress.x) + 0.5, game.world.elevation(game.progress.x, game.progress.z), Float(game.progress.z) + 0.5)
        renderer.facing = game.progress.facing
        renderer.follower = game.player?.species
        if reset { renderer.followerLocation = renderer.location + SIMD3(1, 0, 0); renderer.resetCamera() }
    }
    private func sync() {
        let battle = game.progress.battle
        titleLabel.text = game.world.region(game.progress.x)
        objective.text = game.player == nil ? "前往研究所 · 领取你的第一只搭档" : "探索西侧草丛 · 原生移植验证区域"
        miniMap.x = game.progress.x; miniMap.z = game.progress.z; miniMap.setNeedsDisplay()
        miniMap.accessibilityValue = "\(game.world.region(game.progress.x))，位置 \(game.progress.x)，\(game.progress.z)"
        for v in [stick, miniMap!, titleLabel, objective, message, main, bag, team, run, saveButton] { v.isHidden = battle != nil }
        battleView.isHidden = battle == nil
        battleView.update(game: game, playerImage: game.player.flatMap { sprite($0.species, side: "back") }, enemyImage: battle.flatMap { sprite($0.foe.species, side: "front") })
        label(main, game.nearLab ? "交谈" : game.nearHome && game.player != nil ? "休息" : "交互")
        label(run, running ? "行走" : "奔跑"); main.isEnabled = true
        view.setNeedsLayout()
    }
    private func show(_ text: String, seconds: Double = 5) {
        message.text = text; message.accessibilityLabel = text; message.isHidden = game.isBattling
        battleView.showMessage(text)
        messageUntil = CACurrentMediaTime() + seconds; view.setNeedsLayout()
    }
    private func apply(_ messages: [String]) {
        UIImpactFeedbackGenerator(style: .soft).impactOccurred()
        sync(); if !messages.isEmpty { show(messages.joined(separator: "\n")) }
        persist()
    }
    private func battleAction(_ action: BattleView.Action) {
        switch action {
        case .bag: openBag()
        case .party: openParty()
        case .escape: apply(game.escape())
        case .ball: apply(game.capture())
        case .move(let index): apply(game.attack(index)); battleView.animateAttack()
        case .leave: game.leaveBattle(); positionCamera(reset: true); sync(); show("继续出发。进度已自动保存。"); persist()
        }
    }
    @objc private func interact() {
        if game.nearLab && game.player == nil { selectStarter() }
        else if game.nearHome && game.player != nil { game.healParty(); apply(["在家休息，队伍体力与招式 PP 已恢复。"] ) }
        else if game.nearLab { show("空木博士：带上搭档，去西边 29 号道路的草丛探索吧。") }
        else { show(game.player == nil ? "研究所在左上方。靠近入口后点“交谈”。" : "在草丛行走可能遇到野生精灵。回家可以恢复体力。") }
    }
    private func selectStarter() {
        stick.reset()
        let alert = UIAlertController(title: "选择你的搭档", message: "原生移植验证从领取御三家开始。", preferredStyle: .alert)
        for id in [152, 155, 158] {
            alert.addAction(UIAlertAction(title: game.rules.species(id).name, style: .default) { [weak self] _ in
                guard let self, self.game.chooseStarter(id) else { return }
                self.positionCamera(reset: true); self.apply(["\(self.game.rules.species(id).name)成为了你的搭档！向西进入草丛吧。"])
            })
        }
        alert.addAction(UIAlertAction(title: "再想一下", style: .cancel)); present(alert, animated: true)
    }
    @objc private func toggleRun() {
        running.toggle(); sync()
    }
    @objc private func openBag() {
        stick.reset()
        let alert = UIAlertController(title: "背包", message: "伤药恢复 20 点体力；先削弱野生精灵，再投球。", preferredStyle: .alert)
        let potion = UIAlertAction(title: "伤药 ×\(game.progress.potions)", style: .default) { [weak self] _ in guard let self else { return }; self.apply(self.game.potion()) }
        potion.isEnabled = game.progress.potions > 0; alert.addAction(potion)
        if game.progress.battle?.result == nil && game.isBattling {
            let ball = UIAlertAction(title: "精灵球 ×\(game.progress.balls)", style: .default) { [weak self] _ in guard let self else { return }; self.apply(self.game.capture()) }
            ball.isEnabled = game.progress.balls > 0 && game.progress.party.count < 6; alert.addAction(ball)
        }
        alert.addAction(UIAlertAction(title: "返回", style: .cancel)); present(alert, animated: true)
    }
    @objc private func openParty() {
        stick.reset()
        let alert = UIAlertController(title: "我的队伍", message: game.progress.party.isEmpty ? "还没有搭档，先去研究所。" : "选择出场精灵。战斗中主动更换会消耗一个回合。当前验证内容支持 Lv.1–10。", preferredStyle: .alert)
        for (index, mon) in game.progress.party.enumerated() {
            let text = "\(index == game.progress.active ? "● " : "")\(game.rules.species(mon.species).name) Lv.\(mon.level)  HP \(mon.hp)/\(mon.stat(0, game.rules))"
            let action = UIAlertAction(title: text, style: .default) { [weak self] _ in guard let self else { return }; self.apply(self.game.switchTo(index)); self.positionCamera(reset: true) }
            action.isEnabled = index != game.progress.active && mon.hp > 0; alert.addAction(action)
        }
        alert.addAction(UIAlertAction(title: "返回", style: .cancel)); present(alert, animated: true)
    }
    @objc private func manualSave() { persist(showSuccess: true) }
    private func persist(showSuccess: Bool = false) {
        guard let game else { return }
        do { try game.save(); if showSuccess { show("进度已保存。战斗中也可以保存并继续。") } }
        catch { show("保存失败：\(error.localizedDescription)", seconds: 10) }
    }
    @objc private func suspendGame() { stick.reset(); metalView.isPaused = true; persist() }
    @objc private func resumeGame() { nextStep = 0; renderer?.resetCamera(); metalView.isPaused = false }
    private func sprite(_ species: Int, side: String) -> UIImage? {
        let key = "pokemon-\(species)-\(side)"
        if let cached = imageCache[key] { return cached }
        guard let source = UIImage(contentsOfFile: resources.appendingPathComponent(key + ".png").path)?.cgImage,
              let frame = source.cropping(to: CGRect(x: 0, y: 0, width: 80, height: 80)) else { return nil }
        var pixel: [UInt8] = [0, 0, 0, 0]
        let color = CGColorSpaceCreateDeviceRGB()
        pixel.withUnsafeMutableBytes { buffer in
            if let context = CGContext(data: buffer.baseAddress, width: 1, height: 1, bitsPerComponent: 8, bytesPerRow: 4, space: color, bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue) {
                context.draw(frame, in: CGRect(x: 0, y: 0, width: 80, height: 80))
            }
        }
        let ranges = pixel.prefix(3).flatMap { [CGFloat($0), CGFloat($0)] }
        let masked = frame.copy(maskingColorComponents: ranges) ?? frame
        let result = UIImage(cgImage: masked); imageCache[key] = result; return result
    }
}
