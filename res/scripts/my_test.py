import peanut

if (peanut.is_key_pressed(peanut.KeyCode.SPACE)):
    print("PYTHON")


def update(dt: float):
    if (peanut.is_key_pressed(peanut.KeyCode.G)):
        print(f"G was pressed. dt is {dt}")


class my_test:
    def __init__(self):
        self.num = 0

    def update(self, dt: float):
        if (peanut.is_key_pressed(peanut.KeyCode.G)):
            self.num += 1
            print(f"num = {self.num}")
