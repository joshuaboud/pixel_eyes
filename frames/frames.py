#!/usr/bin/env python3

import sys
import os
import json
import time
import pygame
from pygame.locals import (
    K_UP,
    K_DOWN,
    K_LEFT,
    K_RIGHT,
    K_ESCAPE,
    K_DELETE,
    K_d,
    K_l,
    K_n,
    K_o,
    K_t,
    K_MINUS,
    K_SPACE,
    KEYDOWN,
    QUIT,
    MOUSEBUTTONUP
)

SCREEN_W = 640
SCREEN_H = 480

FRAME_LEFT = 20
FRAME_TOP = 20
FRAME_W_REAL = 5
FRAME_H_REAL = 6
FRAME_SCALE = 50
FRAME_W = FRAME_W_REAL * FRAME_SCALE
FRAME_H = FRAME_H_REAL * FRAME_SCALE

FRAME_PERIOD = 1.0 / 15

PALETTE_ROWS = 16
NUM_COLOUR = pow(4, 3)
PALETTE_BLOCK_SZ = 20
PALETTE_START = (FRAME_LEFT + FRAME_W + 20, 20)
PALETTE_END = (PALETTE_START[0] + NUM_COLOUR/PALETTE_ROWS*PALETTE_BLOCK_SZ, PALETTE_START[1] + PALETTE_BLOCK_SZ*PALETTE_ROWS)
PALETTE_BOUND = (PALETTE_START, PALETTE_END)

TYPE_START = (20, FRAME_TOP + FRAME_H + 20)
TYPE_END = (TYPE_START[0] + 20, TYPE_START[1] + 20)

BG = (20,20,20)

FRAME_TYPES = ["COPY", "MIRROR", "LEFT", "RIGHT"]

current_emote = 0
current_frame = 0
current_colour = [0,0,0]

def new_frame(copy_frame = None):
    info = {
        "duration": 1,
        "last_frame": 1,
        "type": 0
    }
    pixels = []
    for i in range(FRAME_H_REAL):
        pixels.append([])
        for j in range(FRAME_W_REAL):
            if copy_frame is None:
                pixels[i].append([0,0,0])
            else:
                pixels[i].append(copy_frame["pixels"][i][j])
    return {"info": info, "pixels": pixels}

def clicked_canvas(pos):
    return pos[0] >= FRAME_LEFT and pos[0] < FRAME_LEFT + FRAME_W and pos[1] >= FRAME_TOP and pos[1] < FRAME_TOP + FRAME_H

def clicked_palette(pos):
    return pos[0] >= PALETTE_START[0] and pos[1] >= PALETTE_START[1] and pos[0] < PALETTE_END[0] and pos[1] < PALETTE_END[1]

def clicked_type(pos):
    return pos[0] >= TYPE_START[0] and pos[1] >= TYPE_START[1] and pos[0] < TYPE_END[0] and pos[1] < TYPE_END[1]

def gen_palette(screen):
    ind = 0
    top = PALETTE_START[1]
    for r in range(4):
        for g in range(4):
            for b in range(4):
                screen.fill((r << 6, g << 6, b << 6), (
                    (PALETTE_START[0] + ind*PALETTE_BLOCK_SZ, top),
                    (PALETTE_BLOCK_SZ, PALETTE_BLOCK_SZ)
                ))
                ind = (ind + 1) % (NUM_COLOUR / PALETTE_ROWS)
                if ind == 0:
                    top += PALETTE_BLOCK_SZ

def change_colour(screen, pos):
    global current_colour
    current_colour = [*screen.get_at(pos)]

def print_multiline(message, screen, text, left = None, right = None, top = None, bottom = None):
    offset = 0
    textsurfaces = []
    max_w = 0
    height_sum = 0
    for line in message.split('\n'):
        textsurface = text.render(line, False, (255,255,255))
        textsurfaces.append(textsurface)
        w = textsurface.get_width()
        if w > max_w:
            max_w = w
        height_sum += textsurface.get_height()
    pos = [0,0]
    if left:
        pos[0] = left
    if right:
        pos[0] = screen.get_width() - right - max_w
    if top:
        pos[1] = top
    if bottom:
        pos[1] = screen.get_height() - bottom - height_sum
    for textsurface in textsurfaces:
        screen.blit(textsurface, (pos[0], pos[1] + offset))
        offset += textsurface.get_height()

def show_help(screen, text):
    help_text = """\
Help:
* UP/DN: Change Emote
* L/R: Change Frame
* +/-: inc/dec frame
  duration
* 'n' to insert frame
  after curr pos
* 't' to cycle frame
  type
* 'o' to generate C
* Click palette to
  change colour
* ESC to save & exit\
"""
    print_multiline(help_text, screen, text, right=20, top=20)

def draw_screen(screen, text, emotes):
    global TYPE_END
    frame = emotes[current_emote][current_frame]["pixels"]
    info = emotes[current_emote][current_frame]["info"]
    screen.fill(BG)
    for i in range(len(frame)):
        for j in range(len(frame[i])):
            colour = frame[i][j]
            screen.fill((colour[0], colour[1], colour[2]), pygame.Rect(FRAME_LEFT + j*FRAME_SCALE, FRAME_TOP + i*FRAME_SCALE, FRAME_SCALE, FRAME_SCALE))
    textsurface = text.render(
        f'Emote: {current_emote + 1}/{len(emotes)} Frame: {current_frame + 1}/{len(emotes[current_emote])} (Frame Duration: {info["duration"]}) Last Frame: {info["last_frame"]}', False, (255,255,255))
    screen.blit(textsurface, (20, SCREEN_H - 20))
    textsurface = text.render(
        f'Type: {FRAME_TYPES[emotes[current_emote][current_frame]["info"]["type"]]}',
        False, (255,255,255)
    )
    TYPE_END = (TYPE_START[0] + textsurface.get_width(), TYPE_START[1] + textsurface.get_height())
    screen.blit(textsurface, TYPE_START)
    gen_palette(screen)
    show_help(screen, text)
    pygame.display.flip()

def colour_pixel(frame, pos):
    global current_colour
    x, y = (pos[0] - FRAME_LEFT)//FRAME_SCALE, (pos[1] - FRAME_TOP)//FRAME_SCALE
    if x >= 0 and x < FRAME_W_REAL and y >= 0 and y < FRAME_H_REAL:
        frame[y][x] = current_colour

def cycle_frame_type(frame):
    frame["info"]["type"] = (frame["info"]["type"] + 1) % len(FRAME_TYPES)

def save(emotes):
    save_path = os.path.dirname(sys.argv[0]) + "/save.json"
    with open(save_path, "w") as f:
        json.dump(emotes, f)
        f.close()

def load():
    emotes = None
    save_path = os.path.dirname(sys.argv[0]) + "/save.json"
    if os.path.isfile(save_path):
        with open(save_path, "r") as f:
            emotes = json.load(f)
            f.close()
    return emotes

def generate(emotes):
    out_path = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(sys.argv[0]) + "/frames.c"
    with open(out_path, "w") as f:
        emote_ind = 0
        emote_inds = []
        f.write(
            "/*\n"
            " * This file was autogenerated by frames.py.\n"
            " * To edit the animations shown, run `make edit_frames` from\n"
            " * the root of the git repo.\n"
            " */\n\n"
        )
        f.write("#include \"frames.h\"\n\n")
        f.write("const Frame frames[] = {\n")
        for emote in emotes:
            emote_inds.append(emote_ind)
            for frame in emote:
                last_frame = frame["info"]["last_frame"]
                frame_type = FRAME_TYPES[frame["info"]["type"]]
                duration = frame["info"]["duration"]
                f.write("\t{\n")
                f.write("\t\t.info = {{.last_frame = {}, .type = {}, .duration = {}}},\n".format(last_frame, frame_type, duration))
                f.write("\t\t.pixels = {\n")
                for row in frame["pixels"]:
                    f.write("\t\t\t{")
                    for col in row:
                        f.write('{{0b{0:06b}}}, '.format(col[0] >> 2 | col[1] >> 4 | col[2] >> 6))
                    f.write("},\n")
                f.write("\t\t},\n")
                emote_ind += 1
                f.write("\t},\n")
        f.write("};\n")
        f.write("\n")
        f.write("const Frame *frame_lut[] = {\n")
        for ind in emote_inds:
            f.write(f"\t&frames[{ind}],\n")
        f.write("};\n")
        f.close()

def setup():
    pygame.init()
    screen = pygame.display.set_mode([SCREEN_W, SCREEN_H])
    pygame.font.init()
    text = pygame.font.SysFont('monospace', 16)
    return (screen, text)

def preview_frame(screen, frame):
    frame_type = frame["info"]["type"]
    pixels = frame["pixels"]
    screen.fill(BG)
    second_frame_left = FRAME_LEFT + FRAME_W + 20
    itr_mult = 1
    if FRAME_TYPES[frame_type] == "MIRROR":
        second_frame_left += FRAME_W - FRAME_SCALE
        itr_mult = -1
    for i in range(len(pixels)):
        for j in range(len(pixels[i])):
            colour = pixels[i][j]
            screen.fill((colour[0], colour[1], colour[2]), pygame.Rect(FRAME_LEFT + j*FRAME_SCALE, FRAME_TOP + i*FRAME_SCALE, FRAME_SCALE, FRAME_SCALE))
            screen.fill((colour[0], colour[1], colour[2]), pygame.Rect(second_frame_left + j*FRAME_SCALE*itr_mult, FRAME_TOP + i*FRAME_SCALE, FRAME_SCALE, FRAME_SCALE))
    pygame.display.flip()

def preview(screen, emote):
    frame_ind = 0
    previewing = True
    while previewing:
        for event in pygame.event.get():
            if event.type == KEYDOWN and event.key == K_SPACE:
                return
        frame = emote[frame_ind]
        preview_frame(screen, frame)
        if frame["info"]["last_frame"]:
            frame_ind = 0
        else:
            frame_ind += 1
        time.sleep(FRAME_PERIOD * frame["info"]["duration"])

def loop(screen, text):
    global current_emote
    global current_frame
    running = True
    emotes = load()
    if not emotes:
        emotes = [[new_frame()] for i in range(8)]
    once = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    running = False
                elif event.key == K_UP:
                    current_emote = max([current_emote - 1, 0])
                    current_frame = min([current_frame, len(emotes[current_emote]) - 1])
                elif event.key == K_DOWN:
                    current_emote = min([current_emote + 1, len(emotes) - 1])
                    current_frame = min([current_frame, len(emotes[current_emote]) - 1])
                elif event.key == K_LEFT:
                    current_frame = max([current_frame - 1, 0])
                elif event.key == K_RIGHT:
                    current_frame = min([current_frame + 1, len(emotes[current_emote]) - 1])
                elif event.key == K_n:
                    emotes[current_emote][len(emotes[current_emote]) - 1]["info"]["last_frame"] = 0
                    emotes[current_emote][current_frame]["info"]["last_frame"] = 0
                    if event.unicode == 'N':
                        emotes[current_emote].append(new_frame(emotes[current_emote][current_frame]))
                    else:
                        emotes[current_emote].insert(current_frame + 1, new_frame(emotes[current_emote][current_frame]))
                    emotes[current_emote][len(emotes[current_emote]) - 1]["info"]["last_frame"] = 1
                    current_frame += 1
                elif event.key == K_d or event.key == K_DELETE:
                    if len(emotes[current_emote]) == 1:
                        emotes[current_emote][current_frame] = new_frame()
                    else:
                        del emotes[current_emote][current_frame]
                        if current_frame >= len(emotes[current_emote]):
                            current_frame -= 1
                            emotes[current_emote][current_frame]["info"]["last_frame"] = 1
                elif event.key == K_o:
                    generate(emotes)
                elif event.key == K_t:
                    cycle_frame_type(emotes[current_emote][current_frame])
                elif event.unicode == '+':
                    dur = emotes[current_emote][current_frame]["info"]["duration"]
                    dur = min(dur + 1, 31)
                    emotes[current_emote][current_frame]["info"]["duration"] = dur
                elif event.key == K_MINUS:
                    dur = emotes[current_emote][current_frame]["info"]["duration"]
                    dur = max(dur - 1, 1)
                    emotes[current_emote][current_frame]["info"]["duration"] = dur
                elif event.key == K_SPACE:
                    preview(screen, emotes[current_emote])
                elif event.key == K_l:
                    emotes[current_emote][current_frame]["info"]["last_frame"] = int(not emotes[current_emote][current_frame]["info"]["last_frame"])
        pos = pygame.mouse.get_pos()
        pressed1, pressed2, pressed3 = pygame.mouse.get_pressed()
        if pressed1:
            if clicked_canvas(pos):
                colour_pixel(emotes[current_emote][current_frame]["pixels"], pos)
            elif clicked_palette(pos):
                change_colour(screen, pos)
            elif clicked_type(pos) and once:
                cycle_frame_type(emotes[current_emote][current_frame])
            once = False
        else:
            once = True
        draw_screen(screen, text, emotes)
    save(emotes)

def main():
    screen, text = setup()
    loop(screen, text)
    pygame.quit()

if __name__ == "__main__":
    main()
