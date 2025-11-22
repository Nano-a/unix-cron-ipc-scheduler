# Dossier `scripts/`

Ce dossier contient les scripts utilitaires pour tester, valider et faciliter le développement du projet.

## 📁 Fichiers

### `test_integration_j1.sh` - Script de Test d'Intégration Jalon 1

**Rôle** : Automatise les tests d'intégration pour valider le fonctionnement complet du Jalon 1.

**Fonctionnalités** :
- Compilation du projet
- Création d'une tâche de test
- Démarrage du démon
- Attente de l'exécution
- Vérification des logs créés
- Nettoyage automatique

**Algorithme** :
```
1. Compiler le projet (make clean && make)
2. Créer un répertoire de test temporaire
3. Créer une tâche de test avec :
   - Timing : toutes les minutes
   - Commande : echo "Hello J1"
4. Lancer le démon en arrière-plan
5. Attendre 10 secondes pour l'exécution
6. Vérifier :
   - Existence de times-exitcodes
   - Existence de stdout
   - Contenu de stdout (doit contenir "Hello J1")
   - Existence de stderr (peut être vide)
7. Arrêter le démon
8. Nettoyer les fichiers temporaires
```

**Utilisation** :
```bash
cd projet-systeme-l3
./scripts/test_integration_j1.sh
```

**Sortie** :
- Logs détaillés dans `logs-j1/steps.log`
- Résultats de vérification dans `logs-j1/check.log`
- Logs du démon dans `logs-j1/erraid.log`

**Codes de retour** :
- `0` : Tous les tests passent
- `1` : Échec d'un test

---

## 🧪 Tests Manuels

### Test avec les Exemples d'Arborescences

Pour tester avec les exemples fournis :

```bash
# Extraire un exemple
tar -xzf ../sy5-2025-2026/Projet/exemples-arborescences/exemple-arborescence-1.tar.gz -C /tmp/

# Lancer le démon
./erraid -r /tmp/tmp-username-erraid

# Dans un autre terminal, vérifier les logs
ls -la /tmp/tmp-username-erraid/tasks/*/
```

### Test de Vérification du Format

Pour vérifier le format binaire des fichiers :

```bash
# Vérifier le format de times-exitcodes
hexdump -C /tmp/test/tasks/0/times-exitcodes

# Vérifier le format de timing
hexdump -C /tmp/test/tasks/0/timing

# Vérifier le type de commande
hexdump -C /tmp/test/tasks/0/cmd/type
```

## 📝 Notes

- Les scripts utilisent des répertoires temporaires dans `/tmp/`
- Les scripts nettoient automatiquement après exécution
- Les logs sont conservés dans `logs-j1/` pour analyse
- Les scripts sont compatibles avec bash (POSIX)

## 🔧 Personnalisation

Pour adapter les tests à vos besoins :

1. Modifier les chemins dans les scripts
2. Ajuster les délais d'attente (`sleep`)
3. Ajouter des vérifications supplémentaires
4. Modifier les commandes de test

## ⚠️ Prerequisites

- Le projet doit être compilable (`make` doit fonctionner)
- Les permissions d'écriture dans `/tmp/` et `logs-j1/`
- Bash installé sur le système

