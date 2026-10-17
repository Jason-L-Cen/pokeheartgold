// SPDX-License-Identifier: GPL-3.0-or-later
import UIKit

@main final class AppDelegate: UIResponder, UIApplicationDelegate {
    func application(_ application: UIApplication, configurationForConnecting session: UISceneSession, options: UIScene.ConnectionOptions) -> UISceneConfiguration {
        let configuration = UISceneConfiguration(name: "Native Game", sessionRole: session.role)
        configuration.sceneClass = UIWindowScene.self; configuration.delegateClass = SceneDelegate.self
        return configuration
    }
}
final class SceneDelegate: UIResponder, UIWindowSceneDelegate {
    var window: UIWindow?
    func scene(_ scene: UIScene, willConnectTo session: UISceneSession, options connectionOptions: UIScene.ConnectionOptions) {
        guard let scene = scene as? UIWindowScene else { return }
        let window = UIWindow(windowScene: scene); window.rootViewController = GameController()
        window.makeKeyAndVisible(); self.window = window
    }
}
