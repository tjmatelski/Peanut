import peanut

if (peanut.is_key_pressed(peanut.KeyCode.SPACE)):
    print("PYTHON")


def update(dt: float):
    if (peanut.is_key_pressed(peanut.KeyCode.G)):
        print(f"G was pressed. dt is {dt}")
