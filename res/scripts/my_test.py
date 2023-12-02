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
        self.editor_fields["test_button"] = peanut.EditorButton()

    def __del__(self):
        print("Python Del!!!!!!!!!!!!!!")

    def runtime_begin(self):
        print('Python begin runtime')

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

    def runtime_end(self):
        print('Python end runtime')

    def editor_update(self):
        if (self.editor_fields["test_button"].pressed):
            print('Test button pressed!')
