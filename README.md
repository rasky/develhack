DevelHack 2016
==============

[![Circle CI](https://circleci.com/gh/rasky/develhack.svg?style=svg&circle-token=5702ab47d2dc826475aa9370d930715d55a5ebdb)](https://circleci.com/gh/rasky/develhack)
![As seen on TV](https://img.shields.io/badge/as%20seen-on%20TV!-ff69b4.svg)
![Bugs nope!](https://img.shields.io/badge/bugs-nope!-brightgreen.svg)

This project implements GitHub's [normalized script pattern](http://githubengineering.com/scripts-to-rule-them-all/) so:

* `./script/bootstrap`: Installs required dependencies on your system;
* `./script/setup`: Sets-up the development environment;
* `./script/build`: Builds the project and leaves a ROM in `build/game.nds`;
* `./script/run`: Runs the game using DeSmuME (it must be in your `$PATH`);
