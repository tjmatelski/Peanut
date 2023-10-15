import peanut

if (peanut.is_key_pressed(peanut.KeyCode.SPACE)):
    print("PYTHON")


def update(dt: float):
    if (peanut.is_key_pressed(peanut.KeyCode.G)):
        print(f"G was pressed. dt is {dt}")


class my_test(peanut.PythonScript):
    def __init__(self):
        peanut.PythonScript.__init__(self)
        print("Python Init!!!!!!!!!!!!!!!!")
        self.num = 0

    def __del__(self):
        print("Python Del!!!!!!!!!!!!!!")

    def update(self, dt):
        if (peanut.is_key_pressed(peanut.KeyCode.G)):
            self.num += 1
            print(f"num = {self.num}")
        if (peanut.is_key_pressed(peanut.KeyCode.W)):
            self.transform().translation.x += 5.0 * dt
