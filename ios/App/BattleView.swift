// SPDX-License-Identifier: GPL-3.0-or-later
import UIKit

private let battleInk = UIColor(red: 0.15, green: 0.18, blue: 0.22, alpha: 1)

private final class HealthBox: UIView {
    private let art: UIImageView, name = UILabel(), level = UILabel(), numbers = UILabel()
    private let hp = UIView(), hpLight = UIView(), experience = UIView()
    private let player: Bool
    private var fraction = 1.0, xp = 0.0
    init(image: UIImage, player: Bool) {
        self.player = player; art = UIImageView(image: image); super.init(frame: .zero)
        art.layer.magnificationFilter = .nearest; addSubview(art)
        for label in [name, level, numbers] {
            label.textColor = battleInk; label.adjustsFontSizeToFitWidth = true; label.minimumScaleFactor = 0.7
            label.adjustsFontForContentSizeCategory = true; addSubview(label)
        }
        level.textAlignment = .right; numbers.textAlignment = .right; numbers.isHidden = !player
        numbers.backgroundColor = UIColor(white: 230 / 255, alpha: 1)
        addSubview(hp); hp.addSubview(hpLight); addSubview(experience)
        experience.backgroundColor = UIColor(red: 74 / 255, green: 148 / 255, blue: 1, alpha: 1)
        experience.isHidden = !player; isAccessibilityElement = true
    }
    required init?(coder: NSCoder) { fatalError() }
    func update(_ mon: Monster, rules: Rules) {
        let species = rules.species(mon.species), maximum = mon.stat(0, rules)
        name.text = species.name; level.text = "Lv.\(mon.level)"; numbers.text = "\(mon.hp) / \(maximum)"
        fraction = Double(mon.hp) / Double(maximum)
        let low = Game.experience(mon.level, growth: species.growth), high = Game.experience(mon.level + 1, growth: species.growth)
        xp = mon.level >= 10 ? 1 : max(0, min(1, Double(mon.experience - low) / Double(max(1, high - low))))
        hp.backgroundColor = fraction > 0.5 ? UIColor(red: 0, green: 148 / 255, blue: 0, alpha: 1) : fraction > 0.2 ? UIColor(red: 180 / 255, green: 106 / 255, blue: 8 / 255, alpha: 1) : UIColor(red: 172 / 255, green: 49 / 255, blue: 57 / 255, alpha: 1)
        hpLight.backgroundColor = fraction > 0.5 ? UIColor(red: 24 / 255, green: 197 / 255, blue: 32 / 255, alpha: 1) : fraction > 0.2 ? UIColor(red: 1, green: 180 / 255, blue: 0, alpha: 1) : UIColor(red: 1, green: 90 / 255, blue: 41 / 255, alpha: 1)
        accessibilityLabel = "\(player ? "我方" : "野生的")\(species.name)，等级 \(mon.level)"
        accessibilityValue = "体力 \(mon.hp) / \(maximum)"
        setNeedsLayout()
    }
    override func layoutSubviews() {
        super.layoutSubviews(); art.frame = bounds
        let s = bounds.width / 128
        func rect(_ x: CGFloat, _ y: CGFloat, _ w: CGFloat, _ h: CGFloat) -> CGRect { CGRect(x: x * s, y: y * s, width: w * s, height: h * s) }
        name.frame = rect(player ? 20 : 6, 5, 69, 15); level.frame = rect(player ? 88 : 82, 5, player ? 30 : 24, 15)
        numbers.frame = rect(65, 30, 54, 12)
        for label in [name, level, numbers] { label.font = UIFontMetrics(forTextStyle: .subheadline).scaledFont(for: .systemFont(ofSize: min(19, 9 * s), weight: .semibold), maximumPointSize: 24) }
        hp.frame = rect(player ? 72 : 56, player ? 25 : 24, CGFloat(floor(fraction * 48)), 3)
        hpLight.frame = CGRect(x: 0, y: s, width: hp.bounds.width, height: 2 * s)
        experience.frame = rect(24, 44, CGFloat(floor(xp * 96)), 2)
    }
}

final class BattleView: UIView {
    enum Action { case bag, party, escape, ball, move(Int), leave }
    var action: ((Action) -> Void)?
    private let backdrop: UIImageView, playerGround: UIImageView, enemyGround: UIImageView
    private let playerSprite = UIImageView(), enemySprite = UIImageView(), arena = UIView(), panel = UIView()
    private let playerHealth: HealthBox, enemyHealth: HealthBox, log = UITextView()
    private let shortcut = UIButton(type: .system), back = UIButton(type: .system)
    private let spriteBottoms: [String: Double]
    private var buttons: [UIButton] = [], art: [String: UIImage] = [:], game: Game?
    private var selectingMove = false

    init(resources: URL) throws {
        var images: [String: UIImage] = [:]
        for name in ["background-day", "background-evening", "background-night", "health-player", "health-enemy", "ball",
                     "button-fight", "button-bag", "button-party", "button-escape", "button-move"] + ["player", "enemy"].flatMap({ side in ["day", "evening", "night"].map { "ground-\(side)-\($0)" } }) {
            let url = resources.appendingPathComponent("battle-\(name).png")
            guard let image = UIImage(contentsOfFile: url.path) else { throw CocoaError(.fileReadCorruptFile, userInfo: [NSFilePathErrorKey: url.path]) }
            images[name] = image
        }
        art = images; backdrop = UIImageView(image: images["background-day"])
        playerGround = UIImageView(image: images["ground-player-day"]); enemyGround = UIImageView(image: images["ground-enemy-day"])
        playerHealth = HealthBox(image: images["health-player"]!, player: true)
        enemyHealth = HealthBox(image: images["health-enemy"]!, player: false)
        spriteBottoms = try JSONDecoder().decode([String: Double].self, from: Data(contentsOf: resources.appendingPathComponent("battle-sprite-bottoms.json")))
        super.init(frame: .zero)
        backgroundColor = battleInk; addSubview(arena); arena.clipsToBounds = true
        for image in [backdrop, playerGround, enemyGround, playerSprite, enemySprite] {
            image.layer.magnificationFilter = .nearest; image.layer.minificationFilter = .nearest
            image.isAccessibilityElement = false; arena.addSubview(image)
        }
        playerSprite.contentMode = .scaleAspectFit; enemySprite.contentMode = .scaleAspectFit
        arena.addSubview(playerHealth); arena.addSubview(enemyHealth)
        log.backgroundColor = UIColor(red: 0.97, green: 0.97, blue: 0.93, alpha: 1)
        log.textColor = battleInk; log.isEditable = false; log.isSelectable = false
        log.textContainerInset = UIEdgeInsets(top: 7, left: 14, bottom: 7, right: 14)
        log.font = .preferredFont(forTextStyle: .subheadline); log.adjustsFontForContentSizeCategory = true
        log.accessibilityLabel = "战斗消息"; addSubview(log)
        panel.backgroundColor = battleInk; addSubview(panel)
        for index in 0..<4 {
            let button = UIButton(type: .system); button.tag = index
            button.addTarget(self, action: #selector(selectButton(_:)), for: .touchUpInside)
            panel.addSubview(button); buttons.append(button)
        }
        for button in [shortcut, back] {
            panel.addSubview(button)
        }
        shortcut.addTarget(self, action: #selector(throwBall), for: .touchUpInside)
        back.addTarget(self, action: #selector(goBack), for: .touchUpInside)
        back.accessibilityLabel = "返回战斗主菜单"; back.accessibilityHint = "不会消耗回合"
    }
    required init?(coder: NSCoder) { fatalError() }

    private func configure(_ button: UIButton, title: String, subtitle: String? = nil, image: String, lightText: Bool = true) {
        var config = UIButton.Configuration.plain()
        config.title = title; config.subtitle = subtitle; config.titleAlignment = .center
        config.baseForegroundColor = lightText ? .white : battleInk
        config.background.image = art["button-\(image)"]!.resizableImage(withCapInsets: UIEdgeInsets(top: 5, left: 7, bottom: 5, right: 7), resizingMode: .stretch)
        config.contentInsets = NSDirectionalEdgeInsets(top: 4, leading: 12, bottom: 4, trailing: 12)
        config.titleTextAttributesTransformer = UIConfigurationTextAttributesTransformer { original in
            var value = original
            value.font = UIFontMetrics(forTextStyle: .headline).scaledFont(for: .systemFont(ofSize: 18, weight: .bold), maximumPointSize: 24)
            return value
        }
        config.subtitleTextAttributesTransformer = UIConfigurationTextAttributesTransformer { original in
            var value = original; value.font = UIFontMetrics(forTextStyle: .caption1).scaledFont(for: .systemFont(ofSize: 12, weight: .medium), maximumPointSize: 17); return value
        }
        button.configuration = config; button.accessibilityLabel = title; button.accessibilityValue = subtitle
        button.titleLabel?.adjustsFontSizeToFitWidth = true; button.titleLabel?.minimumScaleFactor = 0.7
        button.titleLabel?.shadowColor = lightText ? UIColor.black.withAlphaComponent(0.65) : nil
        button.titleLabel?.shadowOffset = CGSize(width: 0, height: 1)
        button.alpha = button.isEnabled ? 1 : 0.45
    }
    func update(game: Game, playerImage: UIImage?, enemyImage: UIImage?) {
        self.game = game
        guard let battle = game.progress.battle, let player = game.player else { selectingMove = false; return }
        let hour = Calendar.current.component(.hour, from: Date())
        let time = hour < 4 || hour >= 20 ? "night" : hour >= 17 && hour < 20 ? "evening" : "day"
        // shortcut: only Route 29 grass encounters exist here; select other original terrain sets when adding maps.
        backdrop.image = art["background-\(time)"]; playerGround.image = art["ground-player-\(time)"]; enemyGround.image = art["ground-enemy-\(time)"]
        playerSprite.image = playerImage; enemySprite.image = enemyImage
        playerSprite.isHidden = player.hp == 0; enemySprite.isHidden = battle.foe.hp == 0 || battle.result == .caught || battle.result == .escaped
        playerHealth.update(player, rules: game.rules); enemyHealth.update(battle.foe, rules: game.rules)
        if battle.result != nil || player.hp == 0 { selectingMove = false }
        renderMenu()
    }
    func showMessage(_ text: String) {
        log.text = text; log.setContentOffset(.zero, animated: false)
        log.accessibilityValue = text; setNeedsLayout()
    }
    func animateAttack() {
        guard !UIAccessibility.isReduceMotionEnabled else { return }
        layoutIfNeeded()
        UIView.animate(withDuration: 0.12, delay: 0, options: [.autoreverse, .allowUserInteraction]) {
            self.playerSprite.transform = CGAffineTransform(translationX: 8, y: -3)
        } completion: { _ in self.playerSprite.transform = .identity }
    }
    private func renderMenu() {
        guard let game, let battle = game.progress.battle, let player = game.player else { return }
        let ended = battle.result != nil, replacement = player.hp == 0
        shortcut.isHidden = selectingMove || ended || replacement; back.isHidden = !selectingMove
        for (index, button) in buttons.enumerated() {
            button.isHidden = (ended || replacement) && index != 0
            if ended || replacement {
                button.isEnabled = true; configure(button, title: ended ? "继续探索" : "更换宝可梦", image: "party", lightText: false)
                button.accessibilityHint = ended ? "结束战斗，回到地图" : "选择一只可继续战斗的宝可梦"
            } else if selectingMove {
                let struggle = player.moves.allSatisfy { $0.pp == 0 } && index == 0
                let slot = index < player.moves.count ? player.moves[index] : nil
                let move = struggle ? game.rules.move(165) : slot.map { game.rules.move($0.id) }
                button.isEnabled = struggle || ((slot?.pp ?? 0) > 0 && move?.supported == true)
                let types = ["一般", "格斗", "飞行", "毒", "地面", "岩石", "虫", "幽灵", "钢", "未知", "火", "水", "草", "电", "超能力", "冰", "龙", "恶"]
                let details = struggle ? "一般 · 无可用 PP" : move.map { $0.supported ? "\(types[$0.type]) · PP \(slot!.pp) / \($0.pp)" : "效果尚未移植" } ?? "尚未学会"
                configure(button, title: move?.name ?? "—", subtitle: details, image: "move", lightText: false)
                button.accessibilityHint = button.isEnabled ? "使用招式，消耗一个回合" : "无法使用"
            } else {
                button.isEnabled = true
                configure(button, title: ["战斗", "背包", "宝可梦", "脱战"][index], image: ["fight", "bag", "party", "escape"][index], lightText: index != 2)
                button.accessibilityHint = index == 0 ? "选择招式，不会消耗回合" : index == 3 ? "尝试逃离野生战斗" : "打开菜单，不会消耗回合"
            }
        }
        shortcut.isEnabled = game.progress.balls > 0 && game.progress.party.count < 6
        var config = UIButton.Configuration.plain()
        config.title = "精灵球"; config.subtitle = "×\(game.progress.balls)"; config.image = art["ball"]
        config.cornerStyle = .capsule
        config.background.backgroundColor = UIColor(red: 0.96, green: 0.96, blue: 0.93, alpha: 1)
        config.background.strokeColor = UIColor(red: 0.64, green: 0.68, blue: 0.75, alpha: 1); config.background.strokeWidth = 2
        config.imagePlacement = .top; config.imagePadding = 0; config.titleAlignment = .center; config.baseForegroundColor = battleInk
        config.contentInsets = NSDirectionalEdgeInsets(top: 2, leading: 1, bottom: 2, trailing: 1)
        shortcut.configuration = config; shortcut.alpha = shortcut.isEnabled ? 1 : 0.45
        shortcut.accessibilityLabel = "精灵球"; shortcut.accessibilityValue = "剩余 \(game.progress.balls) 个"
        shortcut.accessibilityHint = game.progress.party.count >= 6 ? "队伍已满，无法捕获" : "立即投出一个精灵球，消耗一个回合"
        config.image = UIImage(systemName: "chevron.left"); config.title = "返回"; config.subtitle = nil
        back.configuration = config
        panel.accessibilityElements = ended || replacement ? [buttons[0]] : selectingMove ? [back] + buttons : buttons + [shortcut]
        setNeedsLayout()
    }
    @objc private func selectButton(_ button: UIButton) {
        guard let game, let battle = game.progress.battle else { return }
        if battle.result != nil { action?(.leave) }
        else if game.player?.hp == 0 { action?(.party) }
        else if selectingMove { selectingMove = false; action?(.move(button.tag)) }
        else if button.tag == 0 {
            selectingMove = true; renderMenu()
            UIAccessibility.post(notification: .layoutChanged, argument: buttons.first)
        } else { action?([Action.bag, .party, .escape][button.tag - 1]) }
    }
    @objc private func goBack() {
        selectingMove = false; renderMenu(); UIAccessibility.post(notification: .layoutChanged, argument: buttons.first)
    }
    @objc private func throwBall() { action?(.ball) }
    override func layoutSubviews() {
        super.layoutSubviews()
        let safe = bounds.inset(by: safeAreaInsets), margin: CGFloat = 12, gap: CGFloat = 8
        let row: CGFloat = safe.height > 600 ? 64 : 44, panelHeight = row * 2 + gap + 16
        log.textContainerInset = UIEdgeInsets(top: 7, left: safe.minX + 14, bottom: 7, right: bounds.maxX - safe.maxX + 14)
        let textSize = (log.text ?? "").boundingRect(with: CGSize(width: safe.width - 38, height: .greatestFiniteMagnitude), options: [.usesLineFragmentOrigin, .usesFontLeading], attributes: [.font: log.font!], context: nil)
        let logHeight = min(max(54, safe.height * 0.25), max(36, ceil(textSize.height) + 14))
        panel.frame = CGRect(x: 0, y: safe.maxY - panelHeight, width: bounds.width, height: bounds.maxY - safe.maxY + panelHeight)
        log.frame = CGRect(x: 0, y: panel.frame.minY - logHeight, width: bounds.width, height: logHeight)
        arena.frame = CGRect(x: 0, y: 0, width: bounds.width, height: log.frame.minY)
        backdrop.frame = arena.bounds
        let area = CGRect(x: safe.minX, y: safe.minY, width: safe.width, height: arena.bounds.height - safe.minY)
        let front = min(190, area.height * 0.68), rear = min(240, area.height * 0.90)
        let enemyBottom = CGFloat(spriteBottoms["pokemon-\(game?.progress.battle?.foe.species ?? 163)-front"] ?? 1)
        let playerBottom = CGFloat(spriteBottoms["pokemon-\(game?.player?.species ?? 152)-back"] ?? 1)
        let enemyFeet = area.minY + area.height * 0.48, playerFeet = area.maxY + 3
        enemySprite.frame = CGRect(x: area.minX + area.width * 0.75 - front / 2, y: enemyFeet - front * enemyBottom, width: front, height: front)
        playerSprite.frame = CGRect(x: area.minX + area.width * 0.25 - rear / 2, y: playerFeet - rear * playerBottom, width: rear, height: rear)
        let groundWidth = min(400, area.width * 0.48), enemyWidth = front * 1.45
        playerGround.frame = CGRect(x: playerSprite.center.x - groundWidth / 2, y: playerFeet - rear * 0.11, width: groundWidth, height: groundWidth * playerGround.image!.size.height / playerGround.image!.size.width)
        enemyGround.frame = CGRect(x: enemySprite.center.x - enemyWidth / 2, y: enemyFeet - front * 0.03, width: enemyWidth, height: enemyWidth * enemyGround.image!.size.height / enemyGround.image!.size.width)
        let boxWidth = min(260, area.width * 0.30, area.height * 0.50 * 128 / 47)
        enemyHealth.frame = CGRect(x: area.minX + margin, y: area.minY + margin, width: boxWidth, height: boxWidth * 35 / 128)
        playerHealth.frame = CGRect(x: area.maxX - boxWidth - margin, y: max(area.minY + 56, area.maxY - boxWidth * 47 / 128 - 8), width: boxWidth, height: boxWidth * 47 / 128)
        let circle = min(92, row * 1.8), innerX = safe.minX + margin, innerWidth = safe.width - 2 * margin
        back.frame = CGRect(x: innerX, y: 8 + (2 * row + gap - circle) / 2, width: circle, height: circle)
        shortcut.frame = CGRect(x: safe.maxX - margin - circle, y: back.frame.minY, width: circle, height: circle)
        for button in [back, shortcut] { button.layer.cornerRadius = circle / 2; button.clipsToBounds = true }
        let gridX = selectingMove ? back.frame.maxX + 16 : innerX
        let gridWidth = innerWidth - circle - 16, width = (gridWidth - gap) / 2
        for (index, button) in buttons.enumerated() {
            button.frame = CGRect(x: gridX + CGFloat(index % 2) * (width + gap), y: 8 + CGFloat(index / 2) * (row + gap), width: width, height: row)
            for image in button.subviews.compactMap({ $0 as? UIImageView }) { image.layer.magnificationFilter = .nearest }
        }
        if game?.progress.battle?.result != nil || game?.player?.hp == 0 { buttons[0].frame = CGRect(x: safe.midX - min(360, innerWidth) / 2, y: 8 + (row + gap) / 2, width: min(360, innerWidth), height: row) }
    }
}
