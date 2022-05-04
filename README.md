# Snake game for Nintendo Game &amp; Watch
This is a port of the [EmbSnake](https://gitee.com/slipperstree/EmbSnake) that runs on the Nintendo Game &amp; Watch: Super Mario Bros / Zelda game.

## Before build
You need backup your G&W first. See [game-and-watch-backup](https://github.com/ghidraninja/game-and-watch-backup)
## 1. Build / Flash in command line
Build and flash by `make flash` in `game-and-watch-snake` directory

## 2. Build / Flash / Debug with STM32CubeIDE
Import this project into your STM32CubeIDE workspace by `Import -> General -> Existing Projects into Workspace` and build / debug it.

## 3. Just need the game rom
You can just download [pre builded ELF file](/Release/gw_snake.elf) for test.

## Features
This game contains the following pages.
- Home
- Demo mode (which can play game it self just like G&W mario or zelda TIME MODE does)
- Game mode (which you can play by yourself)
- Game over page (Hiscore cannot be saved in flash by now)
- etc.

## Controls
- Press any button at Home page will start a new game.
- Wait a while at Home page will start demo.
- Swich Game / Demo mode by press GAME / TIME button.
- Other operation will show at the bottom of screen.
- Power off (sleep) by press power button.

## Pictures
- ![Home](/Pics/Home.jpg)
- ![Demo](/Pics/Demo.jpg)
- ![Game](/Pics/Game.jpg)
- ![GameOver](/Pics/GameOver.jpg)

## Tasks
- Some simple sound
- Save hi-score
- Screen bright
- Show a DigitClock at demo mode (snake will eat digit for update time)
- [Done]Sleep when Power button pushed

## Thanks
- This project is based on [game-and-watch-base](https://github.com/ghidraninja/game-and-watch-base).
- This project use some code (power down etc.) from [game-and-watch-retro-go](https://github.com/kbeckmann/game-and-watch-retro-go).
- Thanks _tim_ in [discord](https://discord.gg/Zks9wsscK3) for many helps. He also create a page which lists a lot of fun thing with G&W, you can take a look if you like, [Here is it](https://www.schuerewegen.tk/gnw/)