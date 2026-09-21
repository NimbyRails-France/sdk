# Commandes de mod de haut niveau (kit 0.7.3 en développement)

Le mod fournit des fonctions C++ métier et des messages composés de valeurs.
Le SDK compile son adaptateur dans la DLL : exports Windows, vérification des
tailles, synchronisation avec start/stop et conversion des exceptions restent
dans le SDK. Il n'a aucune connaissance de BAL ou d'un autre système national.

```cpp
#include <nimby/mod.hpp>
struct Request { double value; };
struct Response { double result; };
Response calculate(const Request& request) { return {request.value * 2}; }

nimby::Mod nimby::createMod() {
    static constexpr std::array commands{
        nimby::command<Request, Response, calculate>("example.calculate.v1")
    };
    return {.commands = commands};
}
```

La table et ses noms doivent vivre jusqu'à l'arrêt du mod. Les noms doivent
être uniques. Les callbacks peuvent s'exécuter concurremment ; les données
partagées propres au mod doivent être synchronisées par leur propriétaire.
L'arrêt attend les appels en cours via le verrou de cycle de vie de l'adaptateur.
Un callback ne doit pas rappeler l'export d'arrêt de son propre module.

Les messages doivent être trivialement copiables, sans pointeurs, références,
conteneurs propriétaires, ni objets dépendant d'une allocation dans une autre DLL.
`FixedList<T,N>` et `FixedText<N>` fournissent des valeurs bornées avec contrôle
de capacité. La trivialité est vérifiée à la compilation ; l'absence de pointeurs
reste une obligation du schéma. Chaque message est limité à 1 Mio, chaque table
à 256 commandes. Les buffers de requête/réponse doivent être valides et distincts.
Le SDK copie la requête avant le callback et n'expose pas d'objets C++ propriétaires
à travers la frontière. Les erreurs effacent la réponse.

Ce pont n'est pas un protocole réseau ni un format de sauvegarde : le client et
la DLL doivent partager le même schéma et l'ABI Windows x64. Incrémenter le nom
de commande lors d'un changement de disposition, même si sa taille reste identique.
`std::invalid_argument` devient `InvalidArgument`, les exceptions SDK conservent
leur code et les autres deviennent `InternalError`.

## Hôte de diagnostic

```cpp
#include <nimby/mod_module.hpp>
auto mod = nimby::ModModule::besideExecutable("ExampleMod.dll");
auto response = mod.call<Response>("example.calculate.v1", Request{12});
mod.close();
```

Le SDK charge et démarre la DLL, cherche les exports, puis l'arrête et la
décharge. La destruction appelle aussi `close`. Un arrêt échoué conserve le
module chargé : le décharger pourrait laisser des threads actifs. Un appel après
fermeture échoue. L'hôte n'est pas partageable concurremment ; un seul hôte par
module. Il ne doit pas être utilisé sur une DLL déjà gérée par le loader.
Ce chargement local n'installe et n'injecte rien dans NIMBY Rails.

L'export générique privé est `NRFMod_InvokeV1`. Les anciens exports du loader
restent disponibles. Une commande absente retourne `HooksUnavailable` et une
commande sur un module arrêté `InvalidHandle`. Une réponse métier peut exprimer
une indisponibilité de données sans déclencher une erreur de transport.
