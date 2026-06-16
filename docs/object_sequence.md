# Game Object Sequences

This document describes the runtime sequence of each main object in Zomwar. Each object is handled by its own AK task and receives signals from the screen task (`scr_game_zomwar`), button callbacks, the periodic game tick timer, or other object tasks.

## I. Object Summary

| Object | Task ID | Handler | Main responsibility |
|---|---|---|---|
| Gunner | `ZW_GAME_GUNNER_ID` | `zw_game_gunner_handle()` | Controls the player position and gunner image state. |
| Bullet | `ZW_GAME_BULLET_ID` | `zw_game_bullet_handle()` | Shoots bullets, moves active bullets, and hides them when they exit the screen. |
| Zombie | `ZW_GAME_ZOMBIE_ID` | `zw_game_zombie_handle()` | Spawns zombies, moves them (zigzag, rising from tombstones), checks collision with bullets, and updates score. |
| Bang | `ZW_GAME_BANG_ID` | `zw_game_bang_handle()` | Plays explosion animation after a zombie is hit. |
| Border | `ZW_GAME_BORDER_ID` | `zw_game_border_handle()` | Checks wave level-up and game-over conditions. |
| Car | `ZW_GAME_CAR_ID` | `zw_game_car_handle()` | Lawnmower-style rescue cars that activate when a zombie reaches the left edge or touches a parked car. |
| Tombstone | `ZW_GAME_TOMBSTONE_ID` | `zw_game_tombstone_handle()` | Spawns extra zombies that rise out of active tombstones. |

The screen task posts `ZW_GAME_TIME_TICK` every `ZW_GAME_TIME_TICK_INTERVAL` (100 ms). On each tick the screen task fans out signals to every object task in a fixed order.

## II. Gunner Object Sequence

Gunner owns the player position. The screen task initializes the Gunner object when gameplay starts, then the periodic game tick translates the latched direction (`gunner_dir`) into `ZW_GAME_GUNNER_UP` / `ZW_GAME_GUNNER_DOWN` and always posts `ZW_GAME_GUNNER_UPDATE`. Button callbacks only update `gunner_dir` inside the screen task; they do not post to the Gunner task directly. Movement changes the internal `gunner_y` value, clamps it (`AXIS_Y_GUNNER_MIN`..`AXIS_Y_GUNNER_MAX`), then copies it into the rendered `gunner.y`. `ZW_GAME_GUNNER_UPDATE` also clears the recoil frame by resetting `gunner.action_image` from `2` back to `1` (the recoil frame is raised by the Bullet task on `ZW_GAME_BULLET_SHOOT`).

```mermaid
sequenceDiagram
    autonumber
    actor Btn as Button
    participant Scr as Screen task
    participant Gun as Gunner task

    Note over Scr: SCREEN_ENTRY
    Scr-)Gun: ZW_GAME_GUNNER_SETUP
    activate Gun
    Note right of Gun: init pose at (X, Y)<br/>visible = WHITE, action_image = 1
    deactivate Gun
    Note over Scr: arm 100 ms periodic tick

    Note over Btn,Scr: Button events fire asynchronously (between ticks)
    Btn-)Scr: AC_DISPLAY_BUTTON_UP_PRESSED
    activate Scr
    Note right of Scr: gunner_dir = UP
    deactivate Scr
    Btn-)Scr: AC_DISPLAY_BUTTON_DOWN_PRESSED
    activate Scr
    Note right of Scr: gunner_dir = DOWN
    deactivate Scr
    Btn-)Scr: AC_DISPLAY_BUTTON_*_RELEASED
    activate Scr
    Note right of Scr: if dir matches released key<br/>then dir = NONE
    deactivate Scr

    loop Each ZW_GAME_TIME_TICK
        alt gunner_dir == UP
            Scr-)Gun: ZW_GAME_GUNNER_UP
            activate Gun
            Note right of Gun: gunner_y -= STEP<br/>clamp ≥ AXIS_Y_GUNNER_MIN
            deactivate Gun
        else gunner_dir == DOWN
            Scr-)Gun: ZW_GAME_GUNNER_DOWN
            activate Gun
            Note right of Gun: gunner_y += STEP<br/>clamp ≤ AXIS_Y_GUNNER_MAX
            deactivate Gun
        end
        Scr-)Gun: ZW_GAME_GUNNER_UPDATE
        activate Gun
        Note right of Gun: gunner.y ← gunner_y<br/>clear recoil frame (2 → 1)
        deactivate Gun
    end

    Note over Scr: on ZW_GAME_RESET
    Scr-)Gun: ZW_GAME_GUNNER_RESET
    activate Gun
    Note right of Gun: re-park, visible = BLACK
    deactivate Gun
```

<p align="center"><strong><em>Figure 1:</em></strong> Gunner sequence logic</p>

## III. Bullet Object Sequence

Bullet receives shoot input from the MODE button (only while `zw_game_state == GAME_PLAY`). `ZW_GAME_BULLET_SHOOT` picks the first free slot in `bullet[]`, spawns it at `(gunner.x + 22, gunner.y - 8)`, sets `gunner.action_image = 2` to show the recoil frame, and plays `BUZZER_SOUND_CLICK`. The screen task posts `ZW_GAME_BULLET_RUN` on every game tick so visible bullets keep moving to the right by `STEP_BULLET_AXIS_X`. When a bullet reaches `MAX_AXIS_X_BULLET`, it is hidden and its x position is cleared.

```mermaid
sequenceDiagram
    autonumber
    actor Btn as Button
    participant Scr as Screen task
    participant Bul as Bullet task
    participant Gun as Gunner task
    participant Bz as Buzzer

    Note over Scr: SCREEN_ENTRY
    Scr-)Bul: ZW_GAME_BULLET_SETUP
    activate Bul
    Note right of Bul: bullet[*]: visible=BLACK, x=y=0
    deactivate Bul
    Note over Scr: arm 100 ms periodic tick

    Note over Btn,Scr: MODE button fires asynchronously
    Btn-)Scr: AC_DISPLAY_BUTTON_MODE_PRESSED
    activate Scr
    opt zw_game_state == GAME_PLAY
        Scr-)Bul: ZW_GAME_BULLET_SHOOT
    end
    deactivate Scr
    activate Bul
    Note right of Bul: pick first slot with visible≠WHITE<br/>visible=WHITE, x=gunner.x+22, y=gunner.y-8
    Bul->>+Gun: write gunner.action_image = 2 (recoil)
    Gun-->>-Bul: 
    Bul->>+Bz: BUZZER_PlaySound(CLICK)
    Bz-->>-Bul: 
    deactivate Bul

    loop Each ZW_GAME_TIME_TICK
        Scr-)Bul: ZW_GAME_BULLET_RUN
        activate Bul
        loop for each visible bullet
            Note right of Bul: x += STEP_BULLET_AXIS_X
            alt x ≥ MAX_AXIS_X_BULLET
                Note right of Bul: visible=BLACK, x=0
            end
        end
        deactivate Bul
    end

    Note over Scr: on ZW_GAME_RESET
    Scr-)Bul: ZW_GAME_BULLET_RESET
    activate Bul
    Note right of Bul: bullet[*]: visible=BLACK, x=y=0
    deactivate Bul
```

<p align="center"><strong><em>Figure 2:</em></strong> Bullet sequence logic</p>

## IV. Zombie Object Sequence

Zombie owns the horde state. On `ZW_GAME_ZOMBIE_SETUP` it reads `zw_game_zombie_speed` from `settingsetup.zombie_speed`, hides every slot in `zombie[]`, then spawns the first `NUM_ZOMBIE_INIT` zombies at random `(x, y)` inside the right margin (x in `130..168`, y in `ZOMBIE_Y_MIN..ZOMBIE_Y_MAX`). On every `ZW_GAME_TIME_TICK` the screen task posts `ZW_GAME_ZOMBIE_RUN` followed by `ZW_GAME_ZOMBIE_DETONATOR`. In `RUN`, each visible zombie either rises one pixel up (when it was spawned from a tombstone via `zw_game_zombie_spawn_from_tombstone()` and `rise_ticks > 0`; when `rise_ticks` reaches zero, `rising` is cleared and `zigzag_timer` is re-rolled), or steps left by `zw_game_zombie_speed` (clamped at `-ZOMBIE_MIN_LEFT_OFFSET`), applies a vertical zigzag (`dy` re-rolled to `-1..+1` every `zigzag_timer` ticks, clamped to `ZOMBIE_Y_MIN..ZOMBIE_Y_MAX` and reset to `0` on clamp), and cycles `action_image` through frames `1→2→3`. After moving, the task tops the alive count back up to `NUM_ZOMBIE_INIT` by re-spawning hidden slots. In `DETONATOR`, for every visible non-rising zombie it walks `bullet[]`, calls `zw_game_zombie_check_hit()`, and on a hit hides the bullet (`visible = BLACK`, `x = 0`), calls `zw_game_bang_spawn()`, adds `10` to `zw_game_score`, plays `BUZZER_SOUND_BANG`, and hides the zombie. `ZW_GAME_ZOMBIE_WAVE_SPAWN` (posted from the Border task on level-up) increments `zw_game_zombie_speed` up to `ZOMBIE_SPEED_MAX` and respawns up to `ZOMBIE_WAVE_SPAWN` hidden slots. `ZW_GAME_ZOMBIE_RESET` hides every slot.

```mermaid
sequenceDiagram
    autonumber
    participant Scr as Screen task
    participant Bdr as Border task
    participant Tmb as Tombstone task
    participant Zmb as Zombie task
    participant Bz as Buzzer

    Note over Scr: SCREEN_ENTRY
    Scr-)Zmb: ZW_GAME_ZOMBIE_SETUP
    activate Zmb
    Note right of Zmb: speed = settingsetup.zombie_speed<br/>hide all slots<br/>spawn NUM_ZOMBIE_INIT zombies at random (x, y)
    deactivate Zmb
    Note over Scr: arm 100 ms periodic tick

    loop Each ZW_GAME_TIME_TICK
        Scr-)Zmb: ZW_GAME_ZOMBIE_RUN
        activate Zmb
        loop for each visible zombie
            alt rising
                Note right of Zmb: y--, rise_ticks--<br/>if ticks reach 0 then rising=false and re-roll zigzag_timer<br/>cycle action_image 1→2→3
            else moving left
                Note right of Zmb: x -= speed (clamp at -ZOMBIE_MIN_LEFT_OFFSET)<br/>y += dy, clamp to Y_MIN..Y_MAX, dy=0 on clamp<br/>if zigzag_timer hits 0, re-roll dy in -1..+1<br/>cycle action_image 1→2→3
            end
        end
        opt alive less than NUM_ZOMBIE_INIT
            Note right of Zmb: re-spawn hidden slots until count restored
        end
        deactivate Zmb

        Scr-)Zmb: ZW_GAME_ZOMBIE_DETONATOR
        activate Zmb
        loop for each visible non-rising zombie z, for each visible bullet b
            opt hit detected
                Note right of Zmb: hide bullet (visible=BLACK, x=0)<br/>zw_game_bang_spawn(z.x, z.y)<br/>zw_game_score += 10<br/>hide zombie (visible=BLACK)
                Zmb->>+Bz: BUZZER_PlaySound(BANG)
                Bz-->>-Zmb: 
            end
        end
        deactivate Zmb
    end

    Note over Tmb: during Tombstone's ZW_GAME_TOMBSTONE_SPAWN tick
    Tmb->>+Zmb: zw_game_zombie_spawn_from_tombstone(i, x, y)
    Note right of Zmb: target slot becomes visible=WHITE, rising=true<br/>rise_ticks=ZOMBIE_RISE_TICKS, action_image=1
    Zmb-->>-Tmb: 

    Note over Bdr: during Border's ZW_GAME_LEVEL_UP
    Bdr-)Zmb: ZW_GAME_ZOMBIE_WAVE_SPAWN
    activate Zmb
    Note right of Zmb: speed++ (cap ZOMBIE_SPEED_MAX)<br/>re-spawn up to ZOMBIE_WAVE_SPAWN hidden slots
    deactivate Zmb

    Note over Scr: on ZW_GAME_RESET
    Scr-)Zmb: ZW_GAME_ZOMBIE_RESET
    activate Zmb
    Note right of Zmb: hide all slots
    deactivate Zmb
```

<p align="center"><strong><em>Figure 3:</em></strong> Zombie sequence logic</p>

## V. Car Object Sequence

Car owns the lawnmower-style rescue cars (one slot per lane). On `ZW_GAME_CAR_SETUP` it parks each car at `(AXIS_X_CAR, lane_y[i])`, sets `visible` from the i-th bit of `settingsetup.num_car`, and clears `running`. Each `ZW_GAME_TIME_TICK` the screen task posts `ZW_GAME_CAR_RUN` then `ZW_GAME_CAR_HIT`. In `RUN`, the task walks `zombie[]`: a visible zombie that reaches the left edge (`x <= -ZOMBIE_MIN_LEFT_OFFSET`) wakes the nearest non-running car within `CAR_HIT_RANGE_Y` via `zw_game_car_find_nearest()` — that car starts running, `zw_game_bang_spawn()` plays, `zw_game_score += 10`, `BUZZER_SOUND_BANG` is played, and the zombie is hidden; a zombie still on screen whose nearest car overlaps it (`zw_game_car_check_hit`) wakes that car too. After the activation pass, every visible running car slides right by `CAR_SPEED`, cycles `action_image` through `1→2→3`, and despawns (`visible = false`, `running = false`) once `x > LCD_WIDTH`. In `HIT`, every visible running car walks `zombie[]` and for each overlap calls `zw_game_bang_spawn()`, adds `10` to `zw_game_score`, plays `BUZZER_SOUND_BANG`, and hides the zombie. `ZW_GAME_CAR_RESET` re-parks every lane and clears both `visible` and `running`.

```mermaid
sequenceDiagram
    autonumber
    participant Scr as Screen task
    participant Bdr as Border task
    participant Car as Car task
    participant Bz as Buzzer

    Note over Scr: SCREEN_ENTRY
    Scr-)Car: ZW_GAME_CAR_SETUP
    activate Car
    Note right of Car: for each lane i: re-park at (AXIS_X_CAR, lane_y[i])<br/>running=false, action_image=1<br/>visible = bit i of settingsetup.num_car
    deactivate Car
    Note over Scr: arm 100 ms periodic tick

    loop Each ZW_GAME_TIME_TICK
        Scr-)Car: ZW_GAME_CAR_RUN
        activate Car
        Note right of Car: Phase 1 — activation pass
        loop for each visible zombie z
            alt z reached left edge (z.x <= -ZOMBIE_MIN_LEFT_OFFSET)
                Note right of Car: m = zw_game_car_find_nearest(z.y)
                opt m >= 0
                    Note right of Car: car[m].running = true<br/>zw_game_bang_spawn(z.x, z.y)<br/>zw_game_score += 10<br/>hide z (visible=BLACK)
                    Car->>+Bz: BUZZER_PlaySound(BANG)
                    Bz-->>-Car: 
                end
            else z still on screen
                Note right of Car: m = zw_game_car_find_nearest(z.y)
                opt car found and overlaps z
                    Note right of Car: car[m].running = true<br/>kill deferred to CAR_HIT
                end
            end
        end
        Note right of Car: Phase 2 — advance running cars
        loop for each visible running car c
            Note right of Car: c.x += CAR_SPEED<br/>cycle action_image 1→2→3
            opt c reaches right edge (c.x > LCD_WIDTH)
                Note right of Car: visible=false, running=false
            end
        end
        deactivate Car

        Scr-)Car: ZW_GAME_CAR_HIT
        activate Car
        loop for each visible running car c, for each visible zombie z
            opt hit detected
                Note right of Car: zw_game_bang_spawn(z.x, z.y)<br/>zw_game_score += 10<br/>hide z (visible=BLACK)
                Car->>+Bz: BUZZER_PlaySound(BANG)
                Bz-->>-Car: 
            end
        end
        deactivate Car
    end

    Note over Bdr: during Border's ZW_GAME_CHECK_GAME_OVER
    Bdr->>+Car: zw_game_car_find_nearest(zy)
    Note right of Car: walk car array, return index of<br/>nearest non-running visible car within CAR_HIT_RANGE_Y, else -1
    Car-->>-Bdr: idx

    Note over Scr: on ZW_GAME_RESET
    Scr-)Car: ZW_GAME_CAR_RESET
    activate Car
    Note right of Car: for each lane: re-park, visible=false, running=false
    deactivate Car
```

<p align="center"><strong><em>Figure 4:</em></strong> Car sequence logic</p>


## VI. Tombstone Object Sequence

Tombstone owns the `tombstone[NUM_TOMBSTONE]` array (2 tombstones per lane: group 1 at `x = 65..84` and group 2 at `x = 90..109`). On `ZW_GAME_TOMBSTONE_SETUP` it arms `tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL` and, for each lane, sets `active` from the i-th bit of `settingsetup.tombstone_lane_1` (group 1) and `settingsetup.tombstone_lane_2` (group 2). Each `ZW_GAME_TIME_TICK` the screen task posts `ZW_GAME_TOMBSTONE_SPAWN`: while `tombstone_spawn_timer > 0` the task just decrements it and returns; once it reaches `0` the timer is rearmed and a random index `tidx = rand() % NUM_TOMBSTONE` is rolled. If `tombstone[tidx].active` is false the tick is skipped; otherwise it walks `zombie[]` to find the first hidden slot and calls `zw_game_zombie_spawn_from_tombstone(i, tombstone[tidx].x, lane_y[tombstone[tidx].lane] + SIZE_BITMAP_TOMBSTONE_Y)`, which starts a zombie rising out of the tombstone over `ZOMBIE_RISE_TICKS` frames. `ZW_GAME_TOMBSTONE_RESET` clears the timer and zeros every slot (`x = 0`, `lane = 0`, `active = false`).

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_tombstone_sequence.png" alt="Tombstone game sequence logic" width="900"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 5:</em></strong> Tombstone sequence logic</p>

## VII. Bang Object Sequence

Bang owns the `bang[NUM_BANG]` array of short-lived explosion sprites that play whenever a zombie is killed. It exposes no spawn signal: instead, the Zombie task (on a bullet hit, `zw_game_zombie.cpp`) and the Car task (on car activation at the screen edge, and on a running car overlapping a zombie, `zw_game_car.cpp`) call `zw_game_bang_spawn(x, y)` directly, which walks `bang[]`, picks the first slot whose `visible != WHITE`, and writes `visible = WHITE`, `x = max(x + 5, 0)`, `y = max(y - 2, 0)`, `action_image = 1` — the small `(+5, -2)` offset centers the explosion bitmap over the zombie hit point. On `ZW_GAME_BANG_SETUP` the task calls `zw_game_bang_reset_all()`, clearing every slot to `visible = BLACK`, `action_image = 1`. Each `ZW_GAME_TIME_TICK` the screen task posts `ZW_GAME_BANG_UPDATE`: for every visible slot, if `action_image >= 3` the slot is retired (`action_image = 1`, `visible = BLACK`); otherwise `action_image` is incremented, so each explosion plays frames `1 → 2 → 3` over three ticks (~300 ms at the 100 ms tick interval) before its slot becomes free for the next hit. `ZW_GAME_BANG_RESET` runs the same `zw_game_bang_reset_all()` as setup.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_bang_sequence.png" alt="Bang game sequence logic" width="900"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 6:</em></strong> Bang sequence logic</p>

## VIII. Border Object Sequence

Border owns the game's progression state — `zw_game_score`, `wave_last_score`, `wave_level`, and the wave-warning latch (`wave_warning_active`, `wave_warning_timer`) — and is the sole arbiter of game over. On `ZW_GAME_BORDER_SETUP` it calls `zw_game_border_clear()`, zeroing the score, last-wave score, wave level, warning timer, and warning flag. Each `ZW_GAME_TIME_TICK` the screen task posts three Border signals in order: `ZW_GAME_CHECK_GAME_OVER`, `ZW_GAME_WAVE_CHECK`, then `ZW_GAME_LEVEL_UP`. `ZW_GAME_CHECK_GAME_OVER` walks `zombie[]` and, for every visible zombie that has reached the left edge (`x <= -ZOMBIE_MIN_LEFT_OFFSET`), asks the Car task via `zw_game_car_find_nearest(zombie[i].y)` whether any rescue car is in range; if none is found for any such zombie, Border posts `ZW_GAME_RESET` to the screen task (`AC_TASK_DISPLAY_ID`) and stops scanning. `ZW_GAME_WAVE_CHECK` arms a new wave when the player crosses the next score threshold: if no warning is currently active and `zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL` (200), it sets `wave_warning_active = true` and loads `wave_warning_timer = WARNING_BLINK_DURATION` (30 ticks ≈ 3 s) — the screen task uses that timer to blink the warning bitmap at `WARNING_BLINK_RATE` and to render `wave_level` (`scr_game_zomwar.cpp`). `ZW_GAME_LEVEL_UP` runs only while the warning is active: it counts down `wave_warning_timer` by one each tick and, once the timer hits zero, clears the warning, advances `wave_last_score` by `WAVE_SCORE_INTERVAL`, increments `wave_level`, and posts `ZW_GAME_ZOMBIE_WAVE_SPAWN` to the Zombie task so the next wave spawns and the zombie speed steps up. `ZW_GAME_BORDER_RESET` runs the same `zw_game_border_clear()` as setup.

```mermaid
sequenceDiagram
    autonumber
    participant Zombie as Zombie task
    participant Car as Car task
    participant Screen as Screen task
    participant Border as Border task

    Note over Screen: SCREEN_ENTRY
    Screen->>Border: post ZW_GAME_BORDER_SETUP
    activate Border
    Note right of Border: zw_game_border_clear():<br/>zw_game_score = 0<br/>wave_last_score = 0<br/>wave_warning_timer = 0<br/>wave_warning_active = false<br/>wave_level = 0
    deactivate Border

    Note over Screen: arm 100 ms periodic timer<br/>→ ZW_GAME_TIME_TICK

    loop Each ZW_GAME_TIME_TICK
        Note over Zombie,Car: During their own tick handlers,<br/>on bullet/car kill: zw_game_score += 10<br/>(shared global owned by Border)

        Screen->>Border: post ZW_GAME_CHECK_GAME_OVER
        activate Border
        loop i = 0 .. NUM_ZOMBIE-1
            alt zombie[i].visible == WHITE && zombie[i].x <= -ZOMBIE_MIN_LEFT_OFFSET
                Border->>+Car: call zw_game_car_find_nearest(zombie[i].y)
                Car-->>-Border: idx
                alt idx < 0
                    Border->>Screen: post ZW_GAME_RESET (AC_TASK_DISPLAY_ID)
                    Note over Border: break for-loop
                end
            end
        end
        deactivate Border

        Screen->>Border: post ZW_GAME_WAVE_CHECK
        activate Border
        alt !wave_warning_active && zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL
            Note right of Border: wave_warning_active = true<br/>wave_warning_timer = WARNING_BLINK_DURATION
        end
        deactivate Border

        Screen->>Border: post ZW_GAME_LEVEL_UP
        activate Border
        alt wave_warning_active
            alt wave_warning_timer > 0
                Note right of Border: wave_warning_timer--
            else wave_warning_timer == 0
                Note right of Border: wave_warning_active = false<br/>wave_last_score += WAVE_SCORE_INTERVAL<br/>wave_level++
                Border->>Zombie: post ZW_GAME_ZOMBIE_WAVE_SPAWN
            end
        end
        deactivate Border
    end

    Note over Screen: on ZW_GAME_RESET handling
    Screen->>Border: post ZW_GAME_BORDER_RESET
    activate Border
    Note right of Border: zw_game_border_clear()
    deactivate Border
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/sequence_object/zw_game_border_sequence.png" alt="Border game sequence logic" width="900"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 7:</em></strong> Border sequence logic</p>

## IX. Per-Tick Signal Order
                                                                                                                                                                                                                                                                                
The screen task `scr_game_zomwar` posts the following sequence on every `ZW_GAME_TIME_TICK`:

1. `ZW_GAME_GUNNER_UP` or `ZW_GAME_GUNNER_DOWN` (depending on `gunner_dir`)
2. `ZW_GAME_GUNNER_UPDATE`
3. `ZW_GAME_BULLET_RUN`
4. `ZW_GAME_ZOMBIE_RUN`
5. `ZW_GAME_ZOMBIE_DETONATOR`
6. `ZW_GAME_TOMBSTONE_SPAWN`
7. `ZW_GAME_CAR_RUN`
8. `ZW_GAME_CAR_HIT`
9. `ZW_GAME_BANG_UPDATE`
10. `ZW_GAME_CHECK_GAME_OVER`
11. `ZW_GAME_WAVE_CHECK`
12. `ZW_GAME_LEVEL_UP`

On `SCREEN_ENTRY` the screen task posts the matching `*_SETUP` signals to all object tasks and starts the periodic `ZW_GAME_TIME_TICK` timer. On `ZW_GAME_RESET` it removes the timer, posts the `*_RESET` signals, saves `gamescore.score_now = zw_game_score`, transitions to `GAME_OVER`, plays `BUZZER_SOUND_GOODBYE`, and arms a one-shot `ZW_GAME_EXIT_GAME` timer.

## X. Code References

| Object | Source file | Header file |
|---|---|---|
| Gunner | `application/sources/app/game/game_zomwar/zw_game_gunner.cpp` | `application/sources/app/game/game_zomwar/zw_game_gunner.h` |
| Bullet | `application/sources/app/game/game_zomwar/zw_game_bullet.cpp` | `application/sources/app/game/game_zomwar/zw_game_bullet.h` |
| Zombie | `application/sources/app/game/game_zomwar/zw_game_zombie.cpp` | `application/sources/app/game/game_zomwar/zw_game_zombie.h` |
| Bang | `application/sources/app/game/game_zomwar/zw_game_bang.cpp` | `application/sources/app/game/game_zomwar/zw_game_bang.h` |
| Border | `application/sources/app/game/game_zomwar/zw_game_border.cpp` | `application/sources/app/game/game_zomwar/zw_game_border.h` |
| Car | `application/sources/app/game/game_zomwar/zw_game_car.cpp` | `application/sources/app/game/game_zomwar/zw_game_car.h` |
| Tombstone | `application/sources/app/game/game_zomwar/zw_game_tombstone.cpp` | `application/sources/app/game/game_zomwar/zw_game_tombstone.h` |
| Screen | `application/sources/app/screens/scr_game_zomwar.cpp` | `application/sources/app/screens/scr_game_zomwar.h` |
