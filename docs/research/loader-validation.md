# Chargeur automatique — 13 septembre 2026

Ce jalon fait suite à l'autorisation explicite de charger le SDK dans le jeu.
Il remplace l'état précédent « injection non implémentée ». Aucun hook de fonction
du jeu n'est validé ou activé.

## Mécanisme et vérifications

`src/loader/main.cpp` est le programme autonome ; `src/loader/loader.cpp` contient
la surveillance et le chargement. Les headers internes sont sous `include/loader/`.
Les sources MinHook restent statiques. La DLL du SDK n'utilise ni exceptions ni
RTTI dans le build MinGW ; ses imports vérifiés sont bcrypt, KERNEL32 et msvcrt.
Les outils autonomes peuvent utiliser les runtimes MinGW, conservés près du chargeur.

Le chemin exact et le SHA-256 du jeu sont vérifiés avant chargement et à chaque
nouvelle instance. Le fichier SDK est verrouillé en lecture pendant la surveillance.
Les RVA du bootstrap proviennent de l'export de notre propre DLL, jamais d'une
adresse du jeu supposée. Le chargeur retrouve la base réelle du module distant et
vérifie que le bootstrap se trouve en mémoire image exécutable avec les octets
attendus. Il refuse un SDK déjà chargé depuis un autre chemin.

LoadLibraryW est résolu dans son module système propriétaire, avec chemin et
en-têtes de version contrôlés, puis appelé à son entrée existante. Le chargeur
n'enlève aucune instrumentation présente. Le chemin DLL est la seule allocation
distante : mémoire de données PAGE_READWRITE, sans code généré. Après l'appel,
la base x64 de la DLL est lue dans les modules du processus, pas reconstruite à
partir du code de retour 32 bits du thread.

Un second thread appelle `DWORD WINAPI NimbySdk_Bootstrap(void*)` après la fin de
LoadLibrary, donc hors du verrou du chargeur Windows. Les résultats OK et
ALREADY_INITIALIZED sont distingués des erreurs. Les threads terminés et handles
sont libérés ; une opération dépassant 15 secondes n'est ni tuée ni réessayée.
Le buffer distant potentiellement encore lu est laissé jusqu'à la fin du processus.
L'arrêt du superviseur laisse intentionnellement la DLL dans le jeu jusqu'à sa
fermeture : aucun retrait à chaud n'est encore implémenté.

## Débogage de l'entrée Windows déjà redirigée

Le premier essai sur le jeu PID 61960 a refusé le chargement avant toute allocation
distante, car un contrôle identique à celui du bootstrap comparait aussi les octets
de l'API Windows entre processus. Un breakpoint dans le chargeur, avec le débogueur
CLion, a établi la pile `checked_remote_address -> remote_load_library -> attach`.
Seul le chargeur était suspendu, pas le jeu. Valeurs observées :

- module : KERNEL32.DLL, base 140705147846656, taille 831488 ;
- RVA de LoadLibraryW fourni par GetProcAddress : 257584 ;
- octets locaux : `48 FF 25 C9 C5 04 00 CC CC CC CC CC CC CC CC CC` ;
- octets dans le jeu : `E9 57 13 77 FE 04 00 CC CC CC CC CC CC CC CC CC`.

Fait vérifié : l'entrée du jeu commence par un saut relatif différent. L'identité
du composant responsable n'a pas été déterminée. Le chargeur appelle maintenant
l'export système existant après contrôle du module et de la page, sans détourner
l'appel vers une copie non instrumentée. Le contrôle strict des octets reste
appliqué au bootstrap du SDK. Aucun changement n'a été apporté au code Windows
ou à cette redirection dans le processus du jeu.

## Tests et observation réelle

**Debug et Release : 4/4 tests CTest réussis.** Le test `loader_restart` crée
uniquement des processus témoins dont nous possédons le code. Il vérifie :

- refus d'une empreinte différente sans chargement ;
- refus d'un autre chemin portant le même nom et le même contenu ;
- chargement et initialisation, confirmés depuis le processus cible ;
- absence de second chargement par instance ;
- redémarrage du superviseur devant un SDK déjà initialisé ;
- trois redémarrages de processus avec réattachement ;
- conservation d'un hook témoin préexistant sur LoadLibraryW, effectivement appelé.

Le chargement réel a ensuite réussi dans NIMBY Rails, PID 61960 :
`SDK loaded and initialized; game hooks disabled`.
Le module a été retrouvé dans le processus à
`C:/Users/samsv/CLionProjects/untitled/build/package/bin/NimbyRailsSDK.dll`.
Un nouveau superviseur a reçu `SDK already initialized; game hooks disabled`.
La commande `--stop` a arrêté ce superviseur proprement. Le jeu n'a pas été
redémarré par l'agent ; le test des trois redémarrages concerne le programme témoin.
Aucune validation de gameplay ou de cadence de simulation n'est revendiquée.

L'original reste identifié par
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
Le chargeur ne modifie ni ce fichier ni les sauvegardes et n'installe aucune
persistance au démarrage de Windows. L'identification du fichier ne certifie pas
l'intégrité de toute l'image du jeu en mémoire ; les hooks restent refusés.

## Références Windows

- [LoadLibraryW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw)
- [CreateRemoteThread](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)
- [CreateToolhelp32Snapshot](https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot)
