# ✅ Branches créées pour Pitel - T1.5 et T1.6

**Date** : 13 novembre 2025

## 🌿 Branches créées

### 1. Branche `jalon1`
- **Créée à partir de** : `develop`
- **Objectif** : Point de référence pour le Jalon 1
- **Commit de base** : `a371936` (Resolution du Probleme de linkage...)

### 2. Branche `feature/serialization-complex`
- **Créée à partir de** : `jalon1`
- **Objectif** : Implémentation des tâches T1.5 et T1.6 par Pitel
- **Tâches concernées** :
  - **T1.5** : Module de sérialisation (partie 2 : types complexes)
  - **T1.6** : Module de sérialisation (partie 3 : commandes)

## 📋 Tâches assignées à Pitel

### T1.5 - Module de sérialisation (partie 2 : types complexes)
- **Durée estimée** : 4h
- **Deadline** : 14 novembre 2025, 18h00
- **Fonctions à implémenter** :
  - `write_string` et `read_string` (format : LENGTH uint32 + DATA)
  - `write_timing` et `read_timing` (uint64 + uint32 + uint8)
  - `write_arguments` et `read_arguments` (ARGC + ARGV[])
  - Gestion mémoire (allocation/désallocation pour read_string)

### T1.6 - Module de sérialisation (partie 3 : commandes)
- **Durée estimée** : 4h
- **Deadline** : 16 novembre 2025, 18h00
- **Fonctions à implémenter** :
  - `write_command` et `read_command` (récursif)
  - Gestion des commandes simples (TYPE='SI' + arguments)
  - Gestion des commandes complexes (TYPE + NBCMDS + sous-commandes)
  - `create_simple_command` et `create_sequence_command`
  - `free_command` (libération récursive)

## 🔄 Workflow Git pour Pitel

```bash
# 1. Se placer sur la branche de travail
git checkout feature/serialization-complex

# 2. Travailler sur T1.5
# ... modifications ...

# 3. Commiter T1.5
git add src/serialization.c include/serialization.h
git commit -m "T1.5: Implémentation sérialisation types complexes"

# 4. Travailler sur T1.6
# ... modifications ...

# 5. Commiter T1.6
git add src/serialization.c include/serialization.h
git commit -m "T1.6: Implémentation sérialisation commandes"

# 6. Push vers le serveur
git push origin feature/serialization-complex

# 7. Créer une Merge Request sur GitLab vers jalon1
```

## 📊 État actuel

- ✅ Branche `jalon1` créée
- ✅ Branche `feature/serialization-complex` créée
- ✅ BACKLOG.md mis à jour avec la nouvelle branche
- ✅ Branche actuelle : `feature/serialization-complex`

## ⚠️ Notes importantes

- La branche `feature/serialization-complex` est basée sur `jalon1`
- Pitel doit travailler sur cette branche pour T1.5 et T1.6
- Après validation, merger dans `jalon1`, puis `jalon1` dans `develop`

