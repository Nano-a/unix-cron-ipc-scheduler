#!/bin/bash
# Script de réorganisation de l'historique Git
# Auteur : Assistant IA
# Date : 13 novembre 2025
#
# ⚠️ ATTENTION : Ce script modifie l'historique Git
# ⚠️ Lisez ANALYSE_GIT.md avant d'exécuter ce script
# ⚠️ Assurez-vous d'avoir fait un backup complet

set -e  # Arrêter en cas d'erreur

echo "=========================================="
echo "🔧 RÉORGANISATION DE L'HISTORIQUE GIT"
echo "=========================================="
echo ""

# Couleurs pour les messages
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Fonction pour afficher les erreurs
error() {
    echo -e "${RED}❌ ERREUR: $1${NC}" >&2
    exit 1
}

# Fonction pour afficher les succès
success() {
    echo -e "${GREEN}✅ $1${NC}"
}

# Fonction pour afficher les avertissements
warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# Vérification de l'état du dépôt
echo "📋 Étape 0 : Vérification de l'état du dépôt"
echo "--------------------------------------------"

if ! git status --porcelain | grep -q .; then
    success "Working directory propre"
else
    error "Des modifications non commitées existent. Committez ou stashez-les d'abord."
fi

# Création du backup
echo ""
echo "💾 Étape 1 : Création du backup"
echo "--------------------------------------------"
BACKUP_BRANCH="backup-avant-reorganisation-$(date +%Y%m%d-%H%M%S)"
BACKUP_TAG="backup-tag-$(date +%Y%m%d-%H%M%S)"

git branch "$BACKUP_BRANCH" || error "Impossible de créer la branche de backup"
git tag "$BACKUP_TAG" || error "Impossible de créer le tag de backup"
success "Backup créé : branche '$BACKUP_BRANCH' et tag '$BACKUP_TAG'"
echo "   Pour restaurer : git checkout $BACKUP_BRANCH"

# Récupération des commits distants
echo ""
echo "📥 Étape 2 : Récupération des commits distants"
echo "--------------------------------------------"
git fetch --all --prune || error "Impossible de fetch les données distantes"

# Résolution de la divergence de feature/serialization-complex
echo ""
echo "🔄 Étape 3 : Résolution de la divergence feature/serialization-complex"
echo "--------------------------------------------"
git checkout feature/serialization-complex || error "Impossible de checkout feature/serialization-complex"

# Essayer d'abord avec rebase
if git pull --rebase origin feature/serialization-complex 2>/dev/null; then
    success "Rebase réussi pour feature/serialization-complex"
else
    warning "Rebase échoué, tentative avec merge"
    git pull --no-rebase origin feature/serialization-complex || error "Impossible de pull feature/serialization-complex"
    success "Merge réussi pour feature/serialization-complex"
fi

# Merger feature/architecture dans main
echo ""
echo "🔀 Étape 4 : Merge de feature/architecture dans main"
echo "--------------------------------------------"
git checkout main || error "Impossible de checkout main"
git merge --no-ff feature/architecture -m "Merge feature/architecture into main" || error "Impossible de merger feature/architecture dans main"
success "feature/architecture mergée dans main"

# Recréer develop depuis main
echo ""
echo "🌿 Étape 5 : Recréation de develop depuis main"
echo "--------------------------------------------"
git checkout develop || error "Impossible de checkout develop"

# Sauvegarder la liste des commits uniques
COMMITS_DEVELOP=$(git log --oneline main..develop | awk '{print $1}' | tac)
echo "   Commits à appliquer sur develop :"
echo "$COMMITS_DEVELOP" | while read commit; do
    echo "   - $commit : $(git log --oneline -1 $commit | cut -d' ' -f2-)"
done

# Supprimer et recréer develop
git checkout main || error "Impossible de retourner sur main"
git branch -D develop || warning "develop n'existait pas ou était déjà supprimée"
git checkout -b develop || error "Impossible de créer develop"

# Appliquer les commits dans l'ordre chronologique
echo ""
echo "   Application des commits sur develop..."
for commit in $COMMITS_DEVELOP; do
    if git cherry-pick "$commit" 2>/dev/null; then
        echo "   ✅ $commit appliqué"
    else
        warning "   ⚠️  Conflit sur $commit - résolution manuelle requise"
        echo "   Pour continuer après résolution : git cherry-pick --continue"
        echo "   Pour annuler : git cherry-pick --abort"
        exit 1
    fi
done
success "develop recréée avec tous les commits"

# Recréer jalon1 depuis develop
echo ""
echo "🌿 Étape 6 : Recréation de jalon1 depuis develop"
echo "--------------------------------------------"
git checkout develop || error "Impossible de checkout develop"
git branch -D jalon1 2>/dev/null || warning "jalon1 n'existait pas"
git checkout -b jalon1 || error "Impossible de créer jalon1"
success "jalon1 recréée depuis develop"

# Recréer feature/serialization-complex depuis jalon1
echo ""
echo "🌿 Étape 7 : Recréation de feature/serialization-complex depuis jalon1"
echo "--------------------------------------------"
git checkout jalon1 || error "Impossible de checkout jalon1"

# Identifier les commits de feature/serialization-complex
git checkout "$BACKUP_BRANCH" 2>/dev/null || git checkout feature/serialization-complex
COMMITS_SERIALIZATION=$(git log --oneline jalon1..feature/serialization-complex 2>/dev/null | awk '{print $1}' | tac || echo "")

if [ -z "$COMMITS_SERIALIZATION" ]; then
    # Essayer avec la branche distante
    COMMITS_SERIALIZATION=$(git log --oneline jalon1..origin/feature/serialization-complex 2>/dev/null | awk '{print $1}' | tac || echo "")
fi

git checkout jalon1 || error "Impossible de retourner sur jalon1"
git branch -D feature/serialization-complex 2>/dev/null || warning "feature/serialization-complex n'existait pas"
git checkout -b feature/serialization-complex || error "Impossible de créer feature/serialization-complex"

if [ -n "$COMMITS_SERIALIZATION" ]; then
    echo "   Commits à appliquer sur feature/serialization-complex :"
    echo "$COMMITS_SERIALIZATION" | while read commit; do
        echo "   - $commit : $(git log --oneline -1 $commit 2>/dev/null | cut -d' ' -f2- || echo 'commit non trouvé')"
    done
    
    echo ""
    echo "   Application des commits..."
    for commit in $COMMITS_SERIALIZATION; do
        if git cherry-pick "$commit" 2>/dev/null; then
            echo "   ✅ $commit appliqué"
        else
            warning "   ⚠️  Conflit sur $commit - résolution manuelle requise"
            echo "   Pour continuer après résolution : git cherry-pick --continue"
            echo "   Pour annuler : git cherry-pick --abort"
            exit 1
        fi
    done
    success "feature/serialization-complex recréée avec tous les commits"
else
    warning "Aucun commit unique trouvé pour feature/serialization-complex"
    success "feature/serialization-complex créée (vide pour l'instant)"
fi

# Vérification finale
echo ""
echo "🔍 Étape 8 : Vérification finale"
echo "--------------------------------------------"
echo ""
echo "📊 Arbre Git actuel :"
git log --oneline --graph --all --decorate -15
echo ""
echo "📁 Branches créées :"
git branch -vv | grep -E "(main|develop|jalon1|feature/)"
echo ""
success "Réorganisation terminée !"
echo ""
echo "=========================================="
echo "📝 PROCHAINES ÉTAPES"
echo "=========================================="
echo ""
echo "1. Vérifiez que l'arbre correspond à vos attentes"
echo "2. Testez que le code compile : make clean && make"
echo "3. Vérifiez que tous les fichiers sont présents"
echo "4. Si tout est OK, push avec force-with-lease :"
echo ""
echo "   git push origin main --force-with-lease"
echo "   git push origin develop --force-with-lease"
echo "   git push origin jalon1 --force-with-lease"
echo "   git push origin feature/serialization-complex --force-with-lease"
echo ""
echo "5. Si problème, restaurez le backup :"
echo "   git checkout $BACKUP_BRANCH"
echo ""
echo "=========================================="

