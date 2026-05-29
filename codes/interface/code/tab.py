import tkinter as tk

class TableauAvecLabels:
    def __init__(self, root, labels):
        self.root = root
        self.labels = labels
        self.lignes = len(labels)

        self.tableau_entries = {}
        self.valeur_entries = {}

        for i, texte in enumerate(labels):

            # --- Colonne 1 : cellule du tableau ---
            entry_tableau = tk.Entry(root, width=10)
            entry_tableau.grid(row=i, column=0, padx=5, pady=5)
            entry_tableau.position = i
            entry_tableau.bind("<FocusOut>", self.on_focus_out)
            self.tableau_entries[i] = entry_tableau

            # --- Colonne 2 : label venant de la liste ---
            label = tk.Label(root, text=texte, width=20, anchor="w")
            label.grid(row=i, column=1, padx=5, pady=5)

            # --- Colonne 3 : case valeur correspondante ---
            entry_valeur = tk.Entry(root, width=10)
            entry_valeur.grid(row=i, column=2, padx=5, pady=5)
            self.valeur_entries[i] = entry_valeur

    def on_focus_out(self, event):
        ligne = event.widget.position
        valeur = event.widget.get()
        nom_case = f"case_{ligne}"

        print(f"Modification validée dans {nom_case} → valeur finale : {valeur}")

        # Exemple : mettre la même valeur dans la case de droite
        self.valeur_entries[ligne].delete(0, tk.END)
        self.valeur_entries[ligne].insert(0, valeur)

if __name__ == '__main__':
    # Exemple d'utilisation
    root = tk.Tk()
    root.title("Tableau + Labels + Valeurs")

    liste_labels = ["Nom", "Âge", "Ville", "Pays"]
    app = TableauAvecLabels(root, liste_labels)

    root.mainloop()
