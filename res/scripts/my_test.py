import peanut

class my_test(peanut.PythonScript):
    grav = 9.81

    def __init__(self):
        peanut.PythonScript.__init__(self)
        print("Python Init!!!!!!!!!!!!!!!!")
        self.vel = 0.0
        self.editor_fields["speed"] = 1.23
        self.editor_fields["jump_force"] = 10.0
        self.editor_fields["test_int"] = 123

    def __del__(self):
        print("Python Del!!!!!!!!!!!!!!")

    def update(self, dt):
        speed = self.editor_fields["speed"]
        if (peanut.is_key_pressed(peanut.KeyCode.W)):
            self.transform.translation.z += speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.A)):
            self.transform.translation.x -= speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.S)):
            self.transform.translation.z -= speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.D)):
            self.transform.translation.x += speed * dt
        if (peanut.is_key_pressed(peanut.KeyCode.SPACE)):
            self.vel = self.editor_fields["jump_force"]
        self.transform.translation.y += self.vel * dt + 0.5 * -self.grav * dt * dt
        self.vel += -self.grav * dt
        if (self.transform.translation.y < -1.0):
            self.transform.translation.y = -1.0
