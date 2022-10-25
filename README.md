# Flipper Zero - Ma première application
## But
Apprendre à faire une application pour le FZ (Flipper Zero).

## Démarrage
### Environnement de développement (SDK ou Software Development Kit)
Comme tout programme pour faire une compilation, il lui faut un environnement de
développement bien paramétré. Le micro-logiciel (firmware) du FZ fait
office d'environnement. Voici une liste avec le micro-logiciel d'origine ainsi
que certains de ces forks:
  * [Officiel](https://github.com/flipperdevices/flipperzero-firmware)
  * [Unleashed](https://github.com/DarkFlippers/unleashed-firmware)
  * [RogueMaster](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)

Les étapes suivantes sont fait sous Kali Linux et son reproductible pour la plus
part des système d'exploitation GNU/Linux. Télécharger le micro-logiciel voulu,
le développement ici est fait à partir d'Unleashed mais devrait se réaliser
depuis un des autres du dessus:
```bash
git clone --recursive https://github.com/DarkFlippers/unleashed-firmware
```
[Ici](https://github.com/DarkFlippers/unleashed-firmware#project-structure) se
trouve un aperçu non exhaustif des dossiers et de leurs utilisations. Les
développements personnels comme ici se trouve dans le dossier
**application_user**. Nous allons cloner se dépôt dans se dernier:
```bash
git clone https://github.com/canel-rom1/fz-firstapp applications_user/firstapp
```
Ce dépôt est composé de la **LICENSE**, le **README.md** qui est cette page et de deux
fichiers:
  * **application.fam**: Manifest de l'application. Plus d'info
  [ici](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/AppManifests.md)
  * **box_mover.c**: Le code source du programme

#### VS Code
Configurer l'environnement pour VS Code:
```bash
./fbt vscode_dist
  INSTALL .vscode/c_cpp_properties.json
  INSTALL .vscode/launch.json
  INSTALL .vscode/settings.json
  INSTALL .vscode/tasks.json
```

### Compilation
La compilation est simplifiée grâce à l'outil en ligne de commandes **fbt**
(Flipper Build Tool). Il est écrit en Python3 et utilise le module Scons

(Créer un environnement Python?? et y installer les dépendances???)

Installer les dépendances:
```bash
pip3 install -r scripts/requirements.txt
```

Il existe différentes possibilités pour compiler et téléverser son
micro-logiciel dans le Flipper.

#### Compiler et téléverser
Il est possible de directement compiler le micro-logiciel et de le téléverser
directement dans le dossier de mise à jour du FZ. Après, le FZ se met
en mode DFU pour faire la mise à jour avec le nouveau micro-logiciel fraichement
compilé:
```bash
./fbt COMPACT=1 DEBUG=0 FORCE=1 flash_usb_full
```

## Crédits
Je me suis inspiré des exemples suivants:
  * https://flipper.atmanos.com/docs/overview/intro/
  * https://github.com/mfulz/Flipper-Plugin-Tutorial
  * https://github.com/csBlueChip/FlipperZero_plugin_howto
  * https://github.com/zmactep/flipperzero-hello-world
