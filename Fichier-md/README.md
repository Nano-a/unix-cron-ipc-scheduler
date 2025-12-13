# Dossier `Fichier-md/`

Ce dossier contient toute la **documentation complète** du projet erraid-tadmor, organisée en plusieurs fichiers markdown pour faciliter la navigation.

## 📁 Structure

```
Fichier-md/
├── README.md                    # Ce fichier (index de la documentation)
├── CONTEXTE.md                  # Vue d'ensemble, les 3 phases du projet
├── ARCHITECTURE.md              # Architecture système, communication
├── JALON_1.md                   # Guide complet du Jalon 1
├── JALON_2.md                   # Guide complet du Jalon 2
├── COMMANDES_ERRAID.md          # Toutes les commandes du démon
├── COMMANDES_TADMOR.md          # Toutes les commandes du client
├── TESTS_PROFESSEUR.md          # Tests automatiques (valgrind, etc.)
├── TESTS_MANUELS.md             # Guide de test manuel étape par étape
└── ERREURS.md                   # Gestion des erreurs et dépannage
```

## 📚 Guide de Navigation

### Pour Comprendre le Projet

1. **Commencez par** : [README.md](README.md) - Index général
2. **Puis lisez** : [CONTEXTE.md](CONTEXTE.md) - Vue d'ensemble du projet
3. **Ensuite** : [ARCHITECTURE.md](ARCHITECTURE.md) - Architecture du système

### Pour Utiliser le Projet

- **Démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)

### Pour Tester

- **Tests automatiques** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
- **Tests manuels** : [TESTS_MANUELS.md](TESTS_MANUELS.md)

### Pour Dépanner

- **Erreurs courantes** : [ERREURS.md](ERREURS.md)

---

## 📖 Description des Fichiers

### `README.md`
Index général de la documentation avec liens vers tous les autres fichiers.

### `CONTEXTE.md`
- Vue d'ensemble du projet
- Les 3 phases (Jalon 1, Jalon 2, Rendu Final)
- Comparaison des fonctionnalités par phase

### `ARCHITECTURE.md`
- Architecture du système
- Communication client-démon (tubes nommés)
- Structure de stockage
- Format de sérialisation

### `JALON_1.md`
- Fonctionnement du Jalon 1
- Format des tâches
- Fichiers de log
- Exemples d'utilisation

### `JALON_2.md`
- Fonctionnement du Jalon 2
- Types de requêtes (LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE)
- Format des messages
- Exemples d'utilisation

### `COMMANDES_ERRAID.md`
- Toutes les options du démon (`-r`, `-p`, `-d`, `-h`)
- Comportement détaillé
- Exemples d'utilisation
- Codes de retour

### `COMMANDES_TADMOR.md`
- Toutes les commandes du client (`-l`, `-x`, `-o`, `-e`, `-q`)
- Format de sortie pour chaque commande
- Cas d'erreur
- Exemples complets

### `TESTS_PROFESSEUR.md`
- Description détaillée de tous les tests automatiques
- Prérequis (installation valgrind pour test 12)
- Résultats attendus
- Conditions pour que les tests passent
- Dépannage

### `TESTS_MANUELS.md`
- Guide de test manuel étape par étape
- Configuration (2 terminaux)
- Tests Jalon 1 et Jalon 2
- Checklist de vérification

### `ERREURS.md`
- Erreurs courantes du client
- Erreurs courantes du démon
- Solutions et dépannage
- Codes de retour

---

## 🎯 Utilisation Rapide

### Nouveau sur le Projet ?
1. Lisez [README.md](README.md)
2. Puis [CONTEXTE.md](CONTEXTE.md)
3. Ensuite [ARCHITECTURE.md](ARCHITECTURE.md)

### Vous Voulez Tester ?
1. Consultez [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
2. Ou [TESTS_MANUELS.md](TESTS_MANUELS.md)

### Vous Avez un Problème ?
1. Vérifiez [ERREURS.md](ERREURS.md)
2. Consultez les guides de commandes si nécessaire

---

## 📝 Notes

- Tous les fichiers sont en **Markdown** pour faciliter la lecture
- Les fichiers sont **liés entre eux** pour faciliter la navigation
- La documentation est **complète** et couvre tous les aspects du projet
- Les exemples sont **testés** et fonctionnels

---

## 🔗 Liens Externes

Les spécifications officielles se trouvent dans :
- `../sy5-2025-2026/Projet/enonce.md` - Énoncé complet
- `../sy5-2025-2026/Projet/protocole.md` - Format des messages
- `../sy5-2025-2026/Projet/arborescence.md` - Structure de stockage
- `../sy5-2025-2026/Projet/serialisation.md` - Format de sérialisation
