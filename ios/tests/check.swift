// SPDX-License-Identifier: GPL-3.0-or-later
import Foundation

@main struct NativeCheck {
    static func main() throws {
        let resources = URL(fileURLWithPath: CommandLine.arguments[1])
        let directory = FileManager.default.temporaryDirectory.appendingPathComponent(UUID().uuidString)
        try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: directory) }
        let save = directory.appendingPathComponent("native.json")
        let game = try Game(resources: resources, saveURL: save)
        game.progress.rng = 123456
        assert(game.world.walkable(119, 45) && !game.world.walkable(-1, 0))
        assert(game.rules.move(33).power == 35 && game.rules.species(152).hp == 45)
        assert(Game.stage(10, -6) == 2 && Game.stage(10, 6) == 40)
        assert(Game.effectiveness(12, against: ["TYPE_NORMAL", "TYPE_FLYING"]) == 5)
        assert(Game.damage(level: 5, power: 35, attack: 10, defense: 7, critical: false, stab: true, effectiveness: 10, roll: 85) == 7)
        assert(Game.damage(level: 5, power: 35, attack: 10, defense: 7, critical: false, stab: true, effectiveness: 0, roll: 100) == 0)
        assert(!game.chooseStarter(1))
        let start = game.progress.x
        _ = game.move(dx: 1, dz: 1, time: "day")
        assert(game.progress.x == start)

        func path(to goal: (Int, Int) -> Bool) -> [(Int, Int)] {
            let world = game.world, begin = game.progress.z * world.width + game.progress.x
            var queue = [begin], previous = [begin: begin], cursor = 0, end: Int?
            while cursor < queue.count {
                let node = queue[cursor]; cursor += 1
                let x = node % world.width, z = node / world.width
                if goal(x, z) { end = node; break }
                for (dx, dz) in [(0, 1), (0, -1), (1, 0), (-1, 0)] {
                    let index = (z + dz) * world.width + x + dx
                    if world.walkable(x + dx, z + dz) && previous[index] == nil {
                        previous[index] = node; queue.append(index)
                    }
                }
            }
            assert(end != nil, "Original map collision grid must connect the playable route")
            var result: [(Int, Int)] = [], node = end!
            while node != begin {
                let parent = previous[node]!
                result.append((node % world.width - parent % world.width, node / world.width - parent / world.width))
                node = parent
            }
            return result.reversed()
        }
        for (dx, dz) in path(to: { $0 == 108 && $1 == 43 }) { _ = game.move(dx: dx, dz: dz, time: "day") }
        assert(game.nearLab && game.chooseStarter(152) && !game.chooseStarter(155))
        assert(game.player!.hp == game.player!.stat(0, game.rules))
        for (dx, dz) in path(to: { x, z in x < 96 && game.world.permission(x, z) & 255 == 2 }) {
            _ = game.move(dx: dx, dz: dz, time: "day")
            if game.isBattling { break }
        }
        for attempt in 0..<200 where !game.isBattling {
            let candidates = [(1, 0), (-1, 0), (0, 1), (0, -1)].filter { game.world.walkable(game.progress.x + $0.0, game.progress.z + $0.1) && game.world.permission(game.progress.x + $0.0, game.progress.z + $0.1) & 255 == 2 }
            assert(!candidates.isEmpty)
            let (dx, dz) = candidates[attempt % candidates.count]
            _ = game.move(dx: dx, dz: dz, time: "day")
        }
        assert(game.isBattling && [16, 161, 19].contains(game.progress.battle!.foe.species))
        try game.save()
        let resumed = try Game(resources: resources, saveURL: save)
        let first = game.attack(0), second = resumed.attack(0)
        assert(first == second && game.player!.hp == resumed.player!.hp && game.progress.rng == resumed.progress.rng)
        for _ in 0..<100 where game.progress.battle?.result == nil {
            if game.player!.hp < 10 && game.progress.potions > 0 { _ = game.potion() }
            else { _ = game.attack(0) }
        }
        assert(game.progress.battle?.result == .won && game.progress.victories == 1)
        game.leaveBattle(); game.healParty()
        game.progress.party[0].experience = Game.experience(6, growth: game.rules.species(152).growth) - 1
        var levelUpFoe = game.makeMonster(16, level: 2); levelUpFoe.hp = 1
        game.progress.battle = Battle(foe: levelUpFoe); game.progress.rng = 0
        _ = game.attack(0)
        assert(game.player!.level == 6 && game.player!.moves.contains(where: { $0.id == 75 }))
        game.leaveBattle(); try game.save()
        let restored = try Game(resources: resources, saveURL: save)
        let encoder = JSONEncoder(); encoder.outputFormatting = .sortedKeys
        let restoredData = try encoder.encode(restored.progress), currentData = try encoder.encode(game.progress)
        assert(restoredData == currentData)
        var enemy = game.makeMonster(16, level: 2); enemy.hp = 1
        game.progress.battle = Battle(foe: enemy); game.progress.rng = 100
        let ballCount = game.progress.balls
        _ = game.capture()
        assert(game.progress.battle?.result == .caught && game.progress.party.count == 2 && game.progress.balls == ballCount - 1)
        game.leaveBattle()
        game.progress.battle = Battle(foe: game.makeMonster(19, level: 2))
        game.progress.party[0].hp = 0
        let replacementHP = game.progress.party[1].hp
        try game.save()
        let forcedReload = try Game(resources: resources, saveURL: save)
        assert(forcedReload.loadError == nil)
        _ = game.switchTo(1)
        assert(game.progress.active == 1 && game.player!.hp == replacementHP)
        game.progress.party[1].heal(game.rules)
        game.progress.party[1].hp = 1
        for index in game.progress.party[1].moves.indices { game.progress.party[1].moves[index].pp = 0 }
        game.progress.party[0].heal(game.rules)
        var tieFoe = game.makeMonster(16, level: 1); tieFoe.hp = 1
        game.progress.battle = Battle(foe: tieFoe); game.progress.rng = 0
        _ = game.attack(0)
        assert(game.progress.battle?.result == .won && game.player!.hp == 0)
        game.leaveBattle()
        assert(game.progress.active == 0 && game.player!.hp > 0)

        let old = try Data(contentsOf: save)
        let unwritable = try Game(resources: resources, saveURL: save.appendingPathComponent("child.json"))
        do { try unwritable.save(); assertionFailure("Must reject a save path that is a directory") } catch {}
        let afterFailure = try Data(contentsOf: save)
        assert(afterFailure == old)
        let corrupt = directory.appendingPathComponent("corrupt.json")
        try Data("broken".utf8).write(to: corrupt)
        let protected = try Game(resources: resources, saveURL: corrupt)
        assert(protected.loadError != nil)
        do { try protected.save(); assertionFailure("Corrupt save must not be overwritten") } catch {}
        let unchanged = try String(contentsOf: corrupt, encoding: .utf8)
        assert(unchanged == "broken")
        print("PASS: original map collision path, starter → grass encounter → native battle → save/reload, deterministic mid-battle resume, capture/party, damage math, corrupt-save and write-failure protection")
    }
}
