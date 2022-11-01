# Flipper Zero - Box Mover

## But

Première partie d'une suite de tutoriels pour apprendre à faire une application pour le FZ (Flipper Zero).
La liste du tutoriels se trouve [ici](https://github.com/FlipperFrenchCommunity/Awesome-Flipper-French/tree/main/membre_repo#apprendre-%C3%A0-cr%C3%A9er-une-application).

Pour commencer, nous allons mettre en place l'environnement pour développer
notre application. Après nous allons survoler la compilation du micro-logiciel (*firmware*) et
d'une application pour le FZ. Pour finir par une rapide compréhension de
l'architecture du micro-logiciel et de ces applications. Le code source est
largement commenté pour une meilleure compréhension.

L'application va simplement déplacer un point sur l'affichage par les flèches
saisies par l'utilisateur pour apprendre à utiliser une boucle d'événement et
rafraîchir l'affichage.

## Démarrage

### Environnement de développement (SDK ou *Software Development Kit*)

Comme tout programme pour faire une compilation, il lui faut un environnement de
développement bien paramétré. Le micro-logiciel du FZ fait
office d'environnement. Voici une liste avec le micro-logiciel d'origine ainsi
que certains de ces forks:

* [Officiel](https://github.com/flipperdevices/flipperzero-firmware)
* [Unleashed](https://github.com/DarkFlippers/unleashed-firmware)
* [RogueMaster](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)

Les étapes suivantes sont fait sous Kali Linux et son reproductible pour la plus
part des systèmes d'exploitation GNU/Linux. Télécharger le micro-logiciel voulu,
le développement ici est fait à partir d'Unleashed mais devrait se réaliser
depuis un des autres du dessus:

```bash
git clone --recursive https://github.com/DarkFlippers/unleashed-firmware
```

[Ici](https://github.com/DarkFlippers/unleashed-firmware#project-structure) se
trouve un aperçu non exhaustif des dossiers et de leurs utilisations. Les
développements personnels comme ici se trouve dans le dossier
**application_user**. Nous allons cloner se dépôt dans ce dernier:

```bash
git clone https://github.com/FlipperFrenchCommunity/BoxMover box_mover
```

Ce dépôt est composé de la **LICENSE**, le **README.md** qui est cette page et de deux
fichiers:

* **application.fam**: Manifest de l'application. Plus d'info
  [ici](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/AppManifests.md)
* **box_mover.c**: Le code source du programme

#### Documentation

Pour bien programmer, la documentation est importante. Doxygen va parcourir les fichiers source et lire les commentaires pour générer une documentation aux formats HTML ou LaTeX.

Installation:

```bash
sudo apt install doxygen
```

Analyser et générer la documenation:

```bash
doxygen documentation/Doxyfile
```

Ouvrir avec son navigateur préféré:

```bash
firefox documentation/html/index.html
```

#### VS Code

[Source](https://github.com/DarkFlippers/unleashed-firmware/tree/dev/.vscode)

Configurer l'environnement pour VS Code:

```bash
./fbt vscode_dist
  INSTALL .vscode/c_cpp_properties.json
  INSTALL .vscode/launch.json
  INSTALL .vscode/settings.json
  INSTALL .vscode/tasks.json
```

VS Code dispose de deux raccourcis clavier pour le FZ


| Raccourcis clavier | Fonction                                                                  |
| -------------------- | --------------------------------------------------------------------------- |
| Ctrl + Shift + b   | Ouvre un menu avec divers choix pour la compilation ou le téléversement |
| Ctrl + Shift + d   | Attache une sessions pour le débogage                                    |

### Compilation

Source: [How to build](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToBuild.md),
[Flipper Built Tool](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/fbt.md)

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
en mode DFU pour faire la mise à jour avec le nouveau micro-logiciel fraîchement
compilé:

```bash
./fbt COMPACT=1 DEBUG=0 FORCE=1 flash_usb_full
```

#### Flipper Application Package

[Source](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/AppsOnSDCard.md)

Pour éviter de recompiler le micro-logiciel et toutes ces **fap**, il est
possible de cibler de cibler juste son application:

```bash
./fbt fap_{APPID}
```

exemple pour ce dépôt:

```bash
./fbt fap_box_mover
```

La commande la plus pratique dans le flux de programmation compile l'application, la téléverse puis l'exécute
sur le Flipper:

```bash
./fbt launch_app APPSRC=applications/box_mover/
```

## Présentation

Un programme pour le FZ doit être compilé depuis le micro-logiciel. Il sera
compilé sous la forme d'un paquet qui sera démarrer par le système
d'exploitation (OS) du FZ. Le dossier **applications_user** du dépôt du
micro-logiciel est là pour ajouter les codes sources des applications à compiler
comme FAP. A l'intérieur, il suffit de créer un dossier avec le nom de son
programme. Il est préférable et par convention le nom est composé de lettres et
de chiffre et les espaces sont remplacer par un trait souligné (_). C'est aussi
l'endroit où il faudra cloner le dépôt d'un tiers afin de créer un FAP.

Pour créer le programme, il faut au moins deux fichiers:

* ***Le manifeste***: C'est le fichier **application.fam**. Ce fichier sera utilisé
  lors de la compilation pour intégrer le programme dans la structure du micro-logiciel.
  La documentation se trouve [ici](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/AppManifests.md).
* ***box_mover.c***: Le fichier d'entrée contenant le code source du programme.

Pour la compilation, le point d'entrée (*entrypoint*) est définit dans le
manifeste sous le paramètre **entry_point**. C'est la première fonction qui sera démarrée pour exécuter le
programme. Par convention, le point d'entrée est nommé comme le nom du dossier
suivi de **_app** , exemple ici **box_mover_app**.

### Architecture du micro-logiciel

#### Structure générale

Le micro-logiciel est composé de plusieurs parties. L'image suivante à été trouvée [ici](https://flipper.atmanos.com/docs/overview/intro/).

![Architecture software](./assets/architecture_software.png)

Un service est une librairie dédié au fonctionnement général du flipper. Les
applications utilisent les services comme une couche d'abstraction avec furi et
la couche matériel.

#### Journal (log)

Les journaux (log) sont d'une aide précieuse car l permette d'enregistrer et de diffuser des messages aux développeurs ou aux utilisateurs. Ils peuvent mettre des message pour donner une raison au plantage de l'application. Les développeurs peuvent aussi des informations ou lires certaines variables dans le programmes. Donc aussi très utils pour faire du diagnostique.

Dans le [**coeur de Furi**](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/furi/core/log.h), le préprocesseur fournit des constantes pour simplifier la diffusion de message en fonction d'un niveau d'erreur donnée.

Voici les différents niveaux triés par importance. En fonction d'un niveau, il n'y a que ceux qui sont supérieurs ou égals au niveau qui sont diffusés:

1. **FURI_LOG_E()**: Erreur (Error).
2. **FURI_LOG_W()**: Attention (Warning).
3. **FURI_LOG_I()**: Information.
4. **FURI_LOG_D()**: Diagnostique (Debug).
5. **FURI_LOG_T()**: Trace.

Maintenant, il faut savoir comment lire les messages. La solution la plus simple c'est de se connecter avec un terminal en série (UART via l'USB) sur le FZ. Sous GNU/Linux, divers programmes permettent de le faire comme: **minicom**, **picocom**, **screen** ou **tio**.

```bash
tio /dev/ttyACM0


              _.-------.._                    -,
          .-"```"--..,,_/ /`-,               -,  \ 
       .:"          /:/  /'\  \     ,_...,  `. |  |
      /       ,----/:/  /`\ _\~`_-"`     _;
     '      / /`"""'\ \ \.~`_-'      ,-"'/ 
    |      | |  0    | | .-'      ,/`  /
   |    ,..\ \     ,.-"`       ,/`    /
  ;    :    `/`""\`           ,/--==,/-----,
  |    `-...|        -.___-Z:_______J...---;
  :         `                           _-'
 _L_  _     ___  ___  ___  ___  ____--"`___  _     ___
| __|| |   |_ _|| _ \| _ \| __|| _ \   / __|| |   |_ _|
| _| | |__  | | |  _/|  _/| _| |   /  | (__ | |__  | |
|_|  |____||___||_|  |_|  |___||_|_\   \___||____||___|

Welcome to Flipper Zero Command Line Interface!
Read Manual https://docs.flipperzero.one

Firmware version: my unknown (7f55d41d built on 01-11-2022)

>:
```

Une interface à ligne de commande s'affiche et l'utilisateur peut intéragir avec son FZ directement avec des commandes. Le but n'est pas de détailler cette interface, une aide sommaire est disponnible avec la commande **help** ou **?**.

La commande **log** va afficher tout les message de sortie sur le terminal.

```bash
>: log
Press CTRL+C to stop...
```

#### Structure du service GUI

L'interface graphique (*Graphical User Interface*) est gérée par un service
nommée **GUI** qui se trouve: **[./applications/services/gui/](https://github.com/DarkFlippers/unleashed-firmware/tree/dev/applications/services/gui)**.
Le service GUI est découpé en plusieurs modules.

![Service GUI](./assets/service_gui.png)

Les couches les plus basses représente l'affichage de sortie et les entrées
utilisateur. L'affichage est géré directement par la librairie [u8g2](https://github.com/DarkFlippers/unleashed-firmware/tree/dev/lib/u8g2)
et les entrée au travers du service [input](https://github.com/DarkFlippers/unleashed-firmware/tree/dev/applications/services/input).

Le module **canevas** définit les fonctions pour simplifier la création de
contenu pour l'affichage.

Le module **view_port** gère une vue de l'écran ainsi que ces événements.

### Architecture de ce programme

Pour simplifier la lecture de se premier programme, tout est condensé dans un
seul fichier **.c**. Par convention se premier fichier est nommé comme le nom
de l'application et aussi comme le nom du dossier au quel il appartient.

L'allocation des ressources utilisées ainsi que les variables utiles au programme
sont défini dans une structure. C'est l'état (*state*) du programme. Par convention, elle est nommée en **Camel
Case** avec le nom du programme suivi du mot **State**, l'exemple ici est **BoxMoverState**.

Ensuite le fichier est constitué de 5 fonctions:

* **box_mover_app()**: Le point d'entrée.
* **box_mover_alloc()**: Cette fonction va allouer les ressources utilisées
  pour le programme dans la structure contenant l'état du programme.
* **box_mover_free()**: Cette fonction va libérer les ressources.
* **draw_callback**: Rappel de fonction (*callback*) pour mettre à jour l'affichage.
* **input_callback**: Rappel de fonction lors de la saisie d'une touche.

Un information plus détaillée sur le fonctionnement du code source est commentée
directement dans le fichier source.

## Crédits

Je me suis inspiré des exemples suivants:

* https://flipper.atmanos.com/docs/overview/intro/
* https://github.com/mfulz/Flipper-Plugin-Tutorial
* https://github.com/csBlueChip/FlipperZero_plugin_howto
* https://github.com/zmactep/flipperzero-hello-world
