import peanut


class my_test(peanut.PythonScript):
    speed = 5.0
    grav = 9.81

    def __init__(self):
        peanut.PythonScript.__init__(self)
        print("Python Init!!!!!!!!!!!!!!!!")
        self.vel = 0.0

    def __del__(self):
        print("Python Del!!!!!!!!!!!!!!")

    def update(self, dt):
        if (peanut.is_key_pressed(peanut.KeyCode.W)):
            self.transform.translation.z += self.speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.A)):
            self.transform.translation.x -= self.speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.S)):
            self.transform.translation.z -= self.speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.D)):
            self.transform.translation.x += self.speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.SPACE)):
            self.vel = 10.0
        self.transform.translation.y += self.vel * dt + 0.5 * -self.grav * dt * dt
        self.vel += -self.grav * dt
        if (self.transform.translation.y < -1.0):
            self.transform.translation.y = -1.0
