# Projet-Pacman

Programmation C/C++


## Modification cmake
J'ai défini un standard C11 différent de C23 du fichier cmake de base.
D'après mes recherches, C23 est une version plus récente et comme j'avais des erreurs, il m'a été conseillé de choisir une version de C un peu ancienne et de remonter en fonction légèrement.
Je met le lien : https://www.reddit.com/r/C_Programming/comments/xq58u7/which_version_of_c_do_you_useprefer_and_why/?tl=fr

J'ai ajouté manuellement les répertoires include, SDL2 et src pour que cmake trouve les fichiers d'entête.
J'ai explicitement listé tous les fichiers SDL2 et les fichiers source dans add_executable. Ce qui garantira que cmake compile et inclut tous les fichiers nécessairesd ans l'executable final.

## Modification framework.c

## Installation packages
Backend Remote pour WSL
Pour compiler et executer le projet dans un environnement linux, même depuis windows
Installation des dépendances via vcpkg
Installer SDL2 avec vcpkg 

## Quelques explications
LINK : fatal error LNK1168: impossible d'ouvrir pacman.exe pour écrire
ninja: build stopped: subcommand failed.

Ca m'est arrivé au moins deux fois depuisnle début du projet de tomber sur cette erreur après une modification pour tester le fonctionnement.
La solution cnsiste à supprimer le fichier pacman.exe présent dans le répertoire cmake et de recompiler le programme. Cela fonctionne de nouveau.
