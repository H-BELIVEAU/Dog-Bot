from curses.ascii import isdigit

from codes.interface.code.interface import Interface, np, normalize_angle
from codes.interface.code.serial_manager import SerialManager


def angle_rad_to_deg(angle):
    angle = np.rad2deg(normalize_angle(angle))
    if angle < 0:
        angle += 360
    return angle


class Application(Interface):
    def __init__(self):
        self.serial_manager = SerialManager(115200)
        self.serial_manager.auto_connect()
        super().__init__()

        self.last_angle1 = 0
        self.last_angle2 = 0

        self.serial_manager.send(f":::Servo_FLA:INT:0\n")
        self.serial_manager.send(f":::Servo_FLE:INT:0\n")
    def update_positions(self):
        super().update_positions()

        angle1 = int(angle_rad_to_deg(self.servoA-np.pi))
        angle2 = int(angle_rad_to_deg(self.servoB))
        if 0 <= angle1 < 180:
            self.serial_manager.send(f":::Servo_FRA:{angle1}\n")
        if 0 <= angle2 < 180:
            self.serial_manager.send(f":::Servo_FRE:{angle2}\n")


def test_servo_tab():
    from codes.interface.code.tab import TableauAvecLabels, tk
    import time

    root = tk.Tk()
    root.title("Tableau de contrôle des servomoteurs")

    liste_servos = ['Servo_'+a+b+c for a in "FB" for b in "LR" for c in "SAE"]
    print(liste_servos)
    liste_servos += ["all"]

    class Tableau(TableauAvecLabels):
        def __init__(self, root):
            self.serial_manager = SerialManager(115200)
            self.serial_manager.auto_connect()
            super().__init__(root, liste_servos)
        def on_focus_out(self, event):
            ligne = event.widget.position
            valeur = event.widget.get()
            servo = liste_servos[ligne]

            self.valeur_entries[ligne].delete(0, tk.END)
            self.valeur_entries[ligne].insert(0, valeur)

            if valeur != "" and all([isdigit(c) for c in valeur]) and 0 <= int(valeur) <= 180:
                if servo == "all":
                    self.serial_manager.send(f":::Servo_ALL:INT:{chr(int(valeur))*12}\n")
                else:
                    self.serial_manager.send(f':::{servo}:INT:{int(valeur)}\n')

    tab = Tableau(root)
    root.mainloop()


if __name__ == "__main__":
    test_servo_tab()
