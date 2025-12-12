# Comment Tester le Jalon 2

## Méthode 1 : Utiliser le script de test automatique

### Depuis le répertoire du projet

```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
make clean && make
cd autotests
./run-tadmor-tests-jalon-2.sh
```

### Résultat attendu

Si tous les tests passent, vous verrez :
```
Client: all tests passed
```

Si un test échoue, vous verrez des détails sur le test qui a échoué.

## Méthode 2 : Tester manuellement un test spécifique

### Exemple : Tester le test 1 (liste vide)

```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3

# Créer un répertoire temporaire pour les pipes
RUNDIR=$(mktemp -d /tmp/ajinou/test-XXXX)
PIPESDIR="$RUNDIR/pipes"
mkdir -p "$PIPESDIR"
mkfifo "$PIPESDIR/erraid-request-pipe" "$PIPESDIR/erraid-reply-pipe"
chmod 600 "$PIPESDIR"/*

# Lancer le test
cd autotests
timeout 3 cat tadmor-tests/tadmor-jalon-2-test-1/reply > "$PIPESDIR/erraid-reply-pipe" &
PID1=$!
timeout 3 cat "$PIPESDIR/erraid-request-pipe" > /tmp/req.bin &
PID2=$!

# Exécuter tadmor
cd ..
./tadmor -p "$PIPESDIR" -l

# Nettoyer
kill $PID1 $PID2 2>/dev/null
rm -rf "$RUNDIR" /tmp/req.bin
```

## Méthode 3 : Tester avec les tests originaux du professeur

```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
make clean && make

# Créer un lien symbolique ou copier le projet dans le bon endroit
# Le script attend que le projet soit dans le répertoire parent
cd ../sy5-2025-2026-projet-erraid-autotests
# Le script cherche ../tadmor, donc le projet doit être dans le répertoire parent
# Si votre projet est ailleurs, modifiez le script ou créez un lien
```

## Vérification rapide

Pour vérifier rapidement que tout fonctionne :

```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
make clean && make
cd autotests
timeout 60 ./run-tadmor-tests-jalon-2.sh
```

## Dépannage

### Si les tests échouent

1. Vérifiez que le code compile sans erreurs :
   ```bash
   make clean && make
   ```

2. Vérifiez que les pipes sont créées correctement :
   ```bash
   ls -la /tmp/ajinou/run-*/pipes/
   ```

3. Vérifiez les permissions des pipes (doivent être 600)

### Si vous voyez "Segmentation fault" ou "Core dumped"

- Vérifiez que vous avez bien compilé avec les dernières modifications
- Vérifiez que `opcode_used` est bien défini dans `response_t`

## Tests disponibles

Les tests jalon 2 couvrent :
- **Test 1** : Liste vide
- **Test 2** : Liste avec une tâche simple
- **Test 3** : Liste avec plusieurs tâches
- **Test 4** : Liste avec séquences
- **Test 5** : Liste avec séquences complexes
- **Test 6** : TIMES_EXITCODES
- **Test 7** : STDOUT
- **Test 8** : STDERR
- **Test 9** : Réponse d'erreur
- **Test 10** : Réponse d'erreur
- **Test 11** : Liste avec séquences complexes imbriquées

