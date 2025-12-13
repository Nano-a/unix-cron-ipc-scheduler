# Dossier `logs-j1/`

Ce dossier contient les **logs générés lors des tests du Jalon 1**.

## 📁 Contenu

```
logs-j1/
├── check.log      # Résultats des vérifications
├── erraid.log     # Logs du démon erraid
└── steps.log      # Logs détaillés des étapes de test
```

## 🎯 Rôle

Ce dossier est utilisé par les scripts de test pour enregistrer :
- Les résultats des vérifications
- Les logs du démon pendant les tests
- Les étapes détaillées de l'exécution des tests

## 📝 Fichiers

### `check.log`

Contient les résultats des vérifications effectuées par les tests :
- Existence des fichiers de log
- Contenu des fichiers stdout/stderr
- Codes de retour des commandes
- Timestamps des exécutions

### `erraid.log`

Contient les logs du démon `erraid` pendant les tests :
- Messages de démarrage
- Erreurs éventuelles
- Sortie standard du démon

### `steps.log`

Contient les logs détaillés de chaque étape des tests :
- Compilation
- Création des répertoires
- Démarrage du démon
- Vérifications
- Nettoyage

## 🚀 Utilisation

Les logs sont générés automatiquement lors de l'exécution des scripts de test.

Pour consulter les logs :

```bash
# Voir les résultats des vérifications
cat logs-j1/check.log

# Voir les logs du démon
cat logs-j1/erraid.log

# Voir les étapes détaillées
cat logs-j1/steps.log
```

## 🧹 Nettoyage

Les logs peuvent être supprimés pour repartir à zéro :

```bash
rm -rf logs-j1/*
```

**Note** : Les logs sont régénérés automatiquement lors des prochains tests.

## ⚠️ Notes

- Ce dossier est créé automatiquement si il n'existe pas
- Les logs peuvent être volumineux après plusieurs tests
- Les anciens logs peuvent être supprimés sans problème

