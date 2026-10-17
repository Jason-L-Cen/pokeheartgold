// SPDX-License-Identifier: GPL-3.0-or-later
import Foundation

struct LearnMove: Codable { let move: Int; let level: Int }
struct Species: Codable {
    let id: Int, name: String
    let hp: Int, attack: Int, defense: Int, speed: Int, specialAttack: Int, specialDefense: Int
    let types: [String], expYield: Int, catchRate: Int, growth: String, learnset: [LearnMove]
    var bases: [Int] { [hp, attack, defense, speed, specialAttack, specialDefense] }
}
struct Move: Codable {
    let id: Int, name: String, effect: Int, category: Int, power: Int, type: Int, accuracy: Int, pp: Int, priority: Int
    var supported: Bool { effect == 0 || [43, 45, 39, 111, 75, 98, 165].contains(id) }
}
struct Encounter: Codable { let level: Int, weight: Int; let species: [String: Int] }
struct Rules: Codable {
    let species: [Species], moves: [String: Move], encounterRate: Int, encounters: [Encounter]
    func species(_ id: Int) -> Species { species.first { $0.id == id }! }
    func move(_ id: Int) -> Move { moves[String(id)]! }
}
struct Prop: Codable { let model: String; let position: [Float], rotation: [Float], scale: [Float] }
struct Cell: Codable { let model: String; let x: Int, z: Int, altitude: Int; let props: [Prop] }
struct World: Codable {
    let width: Int, height: Int, originX: Int, originZ: Int, cells: [Cell], permissions: [Int], heights: [Float]
    func permission(_ x: Int, _ z: Int) -> Int {
        guard x >= 0, x < width, z >= 0, z < height else { return 0x8000 }
        return permissions[z * width + x]
    }
    func walkable(_ x: Int, _ z: Int) -> Bool {
        // shortcut: tile blocking and water only; port ledge/bridge event rules before expanding traversal.
        let p = permission(x, z)
        return p & 0x8000 == 0 && ![16, 17, 19, 21].contains(p & 255)
    }
    func elevation(_ x: Int, _ z: Int) -> Float {
        guard x >= 0, x < width, z >= 0, z < height else { return 0 }
        return heights[z * width + x]
    }
    func region(_ x: Int) -> String { x >= 96 ? "若叶镇" : "29 号道路" }
}
struct MoveSlot: Codable { let id: Int; var pp: Int }
struct Monster: Codable {
    let species: Int, ivs: [Int]
    var level: Int, experience: Int, hp: Int, moves: [MoveSlot]
    // shortcut: neutral nature and zero EVs in this slice; port nature/EV state with full party mechanics.
    func stat(_ index: Int, _ rules: Rules) -> Int {
        let base = rules.species(species).bases[index]
        return (2 * base + ivs[index]) * level / 100 + (index == 0 ? level + 10 : 5)
    }
    mutating func heal(_ rules: Rules) {
        hp = stat(0, rules)
        for index in moves.indices { moves[index].pp = rules.move(moves[index].id).pp }
    }
}
enum BattleResult: String, Codable { case won, lost, escaped, caught }
struct Battle: Codable {
    var foe: Monster
    var playerAttack = 0, playerDefense = 0, foeAttack = 0, foeDefense = 0, escapeAttempts = 0
    var result: BattleResult?
}
struct Progress: Codable {
    var version = 1, x = 119, z = 45, facing = 0, active = 0, potions = 5, balls = 5, steps = 0, victories = 0
    var rng: UInt32 = UInt32.random(in: 1...UInt32.max)
    var party: [Monster] = []
    var battle: Battle?
}
enum GameError: Error, LocalizedError {
    case invalidSave
    var errorDescription: String? { "存档无法读取，原文件已保留。此时不会覆盖存档。" }
}

final class Game {
    let rules: Rules, world: World, saveURL: URL
    var progress = Progress()
    private(set) var loadError: Error?
    var player: Monster? { progress.party.isEmpty ? nil : progress.party[progress.active] }
    var isBattling: Bool { progress.battle != nil }

    init(resources: URL, saveURL: URL) throws {
        rules = try JSONDecoder().decode(Rules.self, from: Data(contentsOf: resources.appendingPathComponent("rules.json")))
        world = try JSONDecoder().decode(World.self, from: Data(contentsOf: resources.appendingPathComponent("world.json")))
        self.saveURL = saveURL
        guard world.permissions.count == world.width * world.height, world.heights.count == world.permissions.count else { throw GameError.invalidSave }
        if FileManager.default.fileExists(atPath: saveURL.path) {
            do {
                let data = try Data(contentsOf: saveURL)
                guard data.count < 1_048_576 else { throw GameError.invalidSave }
                let loaded = try JSONDecoder().decode(Progress.self, from: data)
                guard valid(loaded) else { throw GameError.invalidSave }
                progress = loaded
            } catch { loadError = GameError.invalidSave }
        }
    }
    private func valid(_ p: Progress) -> Bool {
        guard p.version == 1, world.walkable(p.x, p.z), (0...3).contains(p.facing),
              p.party.count <= 6, (0...999).contains(p.potions), (0...999).contains(p.balls),
              p.steps >= 0, p.steps < 1_000_000_000, p.victories >= 0, p.victories < 1_000_000_000,
              (p.party.isEmpty ? p.active == 0 : p.party.indices.contains(p.active)) else { return false }
        func mon(_ m: Monster) -> Bool {
            guard rules.species.contains(where: { $0.id == m.species }), (1...10).contains(m.level),
                  m.ivs.count == 6, m.ivs.allSatisfy({ (0...31).contains($0) }),
                  m.experience >= 0, m.experience <= Self.experience(10, growth: rules.species(m.species).growth),
                  (0...m.stat(0, rules)).contains(m.hp), (1...4).contains(m.moves.count) else { return false }
            return Set(m.moves.map(\.id)).count == m.moves.count && m.moves.allSatisfy {
                guard let move = rules.moves[String($0.id)] else { return false }
                return (0...move.pp).contains($0.pp)
            }
        }
        guard p.party.allSatisfy(mon) else { return false }
        if let b = p.battle {
            guard !p.party.isEmpty, mon(b.foe), [b.playerAttack, b.playerDefense, b.foeAttack, b.foeDefense].allSatisfy({ (-6...6).contains($0) }),
                  (0...1000).contains(b.escapeAttempts) else { return false }
            if b.result == nil && (b.foe.hp == 0 || !p.party.contains(where: { $0.hp > 0 })) { return false }
        }
        return true
    }
    func save() throws {
        if let error = loadError { throw error }
        guard valid(progress) else { throw GameError.invalidSave }
        try FileManager.default.createDirectory(at: saveURL.deletingLastPathComponent(), withIntermediateDirectories: true)
        try JSONEncoder().encode(progress).write(to: saveURL, options: .atomic)
    }
    func random(_ bound: Int) -> Int {
        precondition(bound > 0)
        progress.rng = progress.rng &* 0x41C64E6D &+ 0x6073
        return Int(progress.rng >> 16) % bound
    }
    static func experience(_ level: Int, growth: String) -> Int {
        let cube = level * level * level
        return growth == "MEDIUM_SLOW" ? max(0, 6 * cube / 5 - 15 * level * level + 100 * level - 140) : cube
    }
    func makeMonster(_ species: Int, level: Int) -> Monster {
        let s = rules.species(species)
        var known: [Int] = []
        for entry in s.learnset where entry.level <= level && rules.move(entry.move).supported {
            if !known.contains(entry.move) { known.append(entry.move) }
        }
        var mon = Monster(species: species, ivs: (0..<6).map { _ in random(32) }, level: level,
                          experience: Self.experience(level, growth: s.growth), hp: 0,
                          moves: known.suffix(4).map { MoveSlot(id: $0, pp: rules.move($0).pp) })
        mon.heal(rules)
        return mon
    }
    func chooseStarter(_ id: Int) -> Bool {
        guard nearLab, progress.party.isEmpty, [152, 155, 158].contains(id) else { return false }
        progress.party = [makeMonster(id, level: 5)]
        return true
    }
    func move(dx: Int, dz: Int, time: String) -> [String] {
        guard abs(dx) + abs(dz) == 1, !isBattling else { return [] }
        progress.facing = dz > 0 ? 0 : dz < 0 ? 1 : dx < 0 ? 2 : 3
        let x = progress.x + dx, z = progress.z + dz
        guard world.walkable(x, z), !(x == 108 && z == 42) else { return [] }
        if x < 98 && progress.party.isEmpty { return ["先去空木博士研究所领取一只搭档。"] }
        progress.x = x; progress.z = z; progress.steps += 1
        guard !progress.party.isEmpty, x < 96, world.permission(x, z) & 255 == 2,
              random(100) < rules.encounterRate else { return [] }
        var roll = random(100)
        for slot in rules.encounters {
            if roll < slot.weight {
                let species = slot.species[time] ?? slot.species["day"]!
                progress.battle = Battle(foe: makeMonster(species, level: slot.level))
                return ["野生的\(rules.species(species).name)出现了！"]
            }
            roll -= slot.weight
        }
        return []
    }
    // shortcut: two New Bark events; port the full field event system with interiors and story progression.
    var nearLab: Bool { abs(progress.x - 108) + abs(progress.z - 41) <= 3 }
    var nearHome: Bool { abs(progress.x - 119) + abs(progress.z - 44) <= 3 }
    func healParty() { for i in progress.party.indices { progress.party[i].heal(rules) } }
    static func stage(_ value: Int, _ stage: Int) -> Int {
        stage >= 0 ? max(1, value * (2 + stage) / 2) : max(1, value * 2 / (2 - stage))
    }
    static func effectiveness(_ type: Int, against types: [String]) -> Int {
        // shortcut: types present in this seven-species slice; import the full chart before expanding the roster.
        let pairs: [Int: [String: Int]] = [
            0: ["TYPE_GHOST": 0, "TYPE_ROCK": 5, "TYPE_STEEL": 5],
            2: ["TYPE_GRASS": 20, "TYPE_FIGHTING": 20, "TYPE_BUG": 20, "TYPE_ROCK": 5, "TYPE_STEEL": 5, "TYPE_ELECTRIC": 5],
            10: ["TYPE_GRASS": 20, "TYPE_FIRE": 5, "TYPE_WATER": 5],
            11: ["TYPE_FIRE": 20, "TYPE_GRASS": 5, "TYPE_WATER": 5],
            12: ["TYPE_WATER": 20, "TYPE_FIRE": 5, "TYPE_GRASS": 5, "TYPE_FLYING": 5]
        ]
        return Set(types).reduce(10) { $0 * (pairs[type]?[$1] ?? 10) / 10 }
    }
    static func damage(level: Int, power: Int, attack: Int, defense: Int, critical: Bool, stab: Bool, effectiveness: Int, roll: Int) -> Int {
        guard effectiveness > 0 else { return 0 }
        var damage = (2 * level / 5 + 2) * power * attack / max(1, defense) / 50 + 2
        damage *= critical ? 2 : 1
        damage = damage * roll / 100
        if stab { damage = damage * 3 / 2 }
        return max(1, damage * effectiveness / 10)
    }
    func attack(_ slot: Int) -> [String] {
        guard let b = progress.battle, b.result == nil, let player else { return [] }
        guard player.hp > 0 else { return ["请先从队伍选择下一只出场精灵。"] }
        let struggle = player.moves.allSatisfy { $0.pp == 0 }
        guard struggle || (player.moves.indices.contains(slot) && player.moves[slot].pp > 0) else { return ["这个招式没有 PP 了。"] }
        let playerMove = struggle ? 165 : player.moves[slot].id
        guard rules.move(playerMove).supported else { return ["这个招式的效果还未移植。"] }
        // shortcut: wild AI picks its first usable damaging move; replace with original trainer/wild AI as coverage grows.
        let enemySlot = b.foe.moves.firstIndex { $0.pp > 0 && rules.move($0.id).power > 0 }
        let enemyMove = enemySlot.map { b.foe.moves[$0].id } ?? 165
        let priority = rules.move(playerMove).priority - rules.move(enemyMove).priority
        let playerFirst = priority != 0 ? priority > 0 : player.stat(3, rules) == b.foe.stat(3, rules) ? random(2) == 0 : player.stat(3, rules) > b.foe.stat(3, rules)
        var messages: [String] = []
        for isPlayer in playerFirst ? [true, false] : [false, true] {
            guard progress.battle?.result == nil, self.player!.hp > 0, progress.battle!.foe.hp > 0 else { break }
            let chosen = isPlayer ? playerMove : enemySlot.map { b.foe.moves[$0].id } ?? 165
            if isPlayer && !struggle { progress.party[progress.active].moves[slot].pp -= 1 }
            if !isPlayer, let index = enemySlot { progress.battle!.foe.moves[index].pp -= 1 }
            messages += perform(chosen, player: isPlayer)
            messages += resolve()
        }
        return messages
    }
    private func perform(_ id: Int, player isPlayer: Bool) -> [String] {
        let b = progress.battle!, move = rules.move(id)
        let attacker = isPlayer ? player! : b.foe, defender = isPlayer ? b.foe : player!
        let name = rules.species(attacker.species).name
        guard move.accuracy == 0 || random(100) < move.accuracy else { return ["\(name)的\(move.name)没有命中。"] }
        if move.power == 0 {
            switch id {
            case 45:
                if isPlayer { progress.battle!.foeAttack = max(-6, b.foeAttack - 1) }
                else { progress.battle!.playerAttack = max(-6, b.playerAttack - 1) }
                return ["\(name)使用叫声，对手的攻击降低了。"]
            case 43, 39:
                if isPlayer { progress.battle!.foeDefense = max(-6, b.foeDefense - 1) }
                else { progress.battle!.playerDefense = max(-6, b.playerDefense - 1) }
                return ["\(name)使用\(move.name)，对手的防御降低了。"]
            case 111:
                if isPlayer { progress.battle!.playerDefense = min(6, b.playerDefense + 1) }
                else { progress.battle!.foeDefense = min(6, b.foeDefense + 1) }
                return ["\(name)使用变圆，防御提高了。"]
            default: return ["\(name)使用了\(move.name)。"]
            }
        }
        let special = move.category == 1
        let attackStage = isPlayer ? b.playerAttack : b.foeAttack
        let defenseStage = isPlayer ? b.foeDefense : b.playerDefense
        let critical = random(id == 75 ? 8 : 16) == 0
        let a = Self.stage(attacker.stat(special ? 4 : 1, rules), special ? 0 : critical ? max(0, attackStage) : attackStage)
        let d = Self.stage(defender.stat(special ? 5 : 2, rules), special ? 0 : critical ? min(0, defenseStage) : defenseStage)
        let kind = [0: "TYPE_NORMAL", 2: "TYPE_FLYING", 10: "TYPE_FIRE", 11: "TYPE_WATER", 12: "TYPE_GRASS"][move.type] ?? ""
        let effect = id == 165 ? 10 : Self.effectiveness(move.type, against: rules.species(defender.species).types)
        let damage = Self.damage(level: attacker.level, power: move.power, attack: a, defense: d, critical: critical,
                                 stab: id != 165 && rules.species(attacker.species).types.contains(kind), effectiveness: effect, roll: 100 - random(16))
        if isPlayer { progress.battle!.foe.hp = max(0, defender.hp - damage) }
        else { progress.party[progress.active].hp = max(0, defender.hp - damage) }
        if id == 165 {
            let recoil = max(1, attacker.stat(0, rules) / 4)
            if isPlayer { progress.party[progress.active].hp = max(0, player!.hp - recoil) }
            else { progress.battle!.foe.hp = max(0, progress.battle!.foe.hp - recoil) }
        }
        return ["\(name)使用\(move.name)，造成 \(damage) 点伤害。\(critical ? "击中要害！" : "")\(effect > 10 ? "效果拔群！" : effect < 10 ? "效果不太好。" : "")"]
    }
    private func resolve() -> [String] {
        guard let b = progress.battle, b.result == nil else { return [] }
        if player!.hp == 0 {
            if b.foe.hp == 0 && progress.party.contains(where: { $0.hp > 0 }) {
                progress.battle!.result = .won; progress.victories += 1
                return ["双方精灵同时倒下，队伍赢得了战斗。出场精灵未获得经验。"]
            }
            if progress.party.contains(where: { $0.hp > 0 }) { return ["搭档失去了战斗能力。请从队伍选择下一只出场精灵。"] }
            progress.battle!.result = .lost
            return ["搭档失去了战斗能力，返回家中休息吧。"]
        }
        guard b.foe.hp == 0 else { return [] }
        progress.battle!.result = .won; progress.victories += 1
        let gain = rules.species(b.foe.species).expYield * b.foe.level / 7
        let index = progress.active, species = rules.species(player!.species)
        // shortcut: the native slice covers levels 1–10; lift this ceiling after porting later move effects and evolution.
        progress.party[index].experience = min(Self.experience(10, growth: species.growth), player!.experience + gain)
        var messages = ["战斗胜利！获得 \(gain) 点经验。"]
        while player!.level < 10 && player!.experience >= Self.experience(player!.level + 1, growth: species.growth) {
            let oldHP = player!.stat(0, rules)
            progress.party[index].level += 1
            let hpIncrease = player!.stat(0, rules) - oldHP
            progress.party[index].hp += hpIncrease
            messages.append("\(species.name)升到了 Lv.\(player!.level)！")
            for move in species.learnset where move.level == player!.level && rules.move(move.move).supported {
                if progress.party[index].moves.count < 4 && !player!.moves.contains(where: { $0.id == move.move }) {
                    progress.party[index].moves.append(MoveSlot(id: move.move, pp: rules.move(move.move).pp))
                    messages.append("学会了\(rules.move(move.move).name)！")
                }
            }
            for move in species.learnset where move.level == player!.level && !rules.move(move.move).supported {
                messages.append("\(rules.move(move.move).name)的效果尚未接入此验证版本。")
            }
        }
        return messages
    }
    private func enemyTurn() -> [String] {
        guard let b = progress.battle, b.result == nil else { return [] }
        let slot = b.foe.moves.firstIndex { $0.pp > 0 && rules.move($0.id).power > 0 }
        let move = slot.map { b.foe.moves[$0].id } ?? 165
        if let slot { progress.battle!.foe.moves[slot].pp -= 1 }
        return perform(move, player: false) + resolve()
    }
    func potion() -> [String] {
        guard let player, player.hp > 0, player.hp < player.stat(0, rules), progress.potions > 0,
              progress.battle?.result == nil else { return ["现在无需使用伤药。"] }
        progress.potions -= 1
        progress.party[progress.active].hp = min(player.stat(0, rules), player.hp + 20)
        return ["使用伤药，恢复了体力。"] + enemyTurn()
    }
    func capture() -> [String] {
        guard let b = progress.battle, b.result == nil, player!.hp > 0, progress.balls > 0, progress.party.count < 6 else { return ["无法使用精灵球：检查数量和队伍空位。"] }
        progress.balls -= 1
        let maxHP = b.foe.stat(0, rules)
        let a = max(1, (3 * maxHP - 2 * b.foe.hp) * rules.species(b.foe.species).catchRate / (3 * maxHP))
        let threshold = a >= 255 ? 65536 : Int(1048560 / sqrt(sqrt(16711680 / Double(a))))
        for _ in 0..<4 where random(65536) >= threshold { return ["精灵挣脱了精灵球！"] + enemyTurn() }
        progress.party.append(b.foe); progress.battle!.result = .caught
        return ["成功收服\(rules.species(b.foe.species).name)！"]
    }
    func escape() -> [String] {
        guard let b = progress.battle, b.result == nil else { return [] }
        progress.battle!.escapeAttempts = min(1000, b.escapeAttempts + 1)
        let chance = player!.stat(3, rules) * 128 / max(1, b.foe.stat(3, rules)) + 30 * progress.battle!.escapeAttempts
        if chance > 255 || random(256) < chance {
            progress.battle!.result = .escaped
            return ["成功逃离了战斗。"]
        }
        return ["没能逃走！"] + enemyTurn()
    }
    func switchTo(_ index: Int) -> [String] {
        guard progress.party.indices.contains(index), index != progress.active, progress.party[index].hp > 0,
              progress.battle?.result == nil else { return [] }
        let forced = player?.hp == 0
        progress.active = index
        if progress.battle != nil { progress.battle!.playerAttack = 0; progress.battle!.playerDefense = 0 }
        return ["去吧，\(rules.species(progress.party[index].species).name)！"] + (forced ? [] : enemyTurn())
    }
    func leaveBattle() {
        guard let battle = progress.battle, battle.result != nil else { return }
        if battle.result == .lost { progress.x = 119; progress.z = 45; healParty() }
        if player?.hp == 0, let next = progress.party.firstIndex(where: { $0.hp > 0 }) { progress.active = next }
        progress.battle = nil
    }
}
