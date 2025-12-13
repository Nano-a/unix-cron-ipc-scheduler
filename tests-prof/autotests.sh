#!/bin/bash

if ! git pull 2>/dev/null >/dev/null; then
  printf "Attention: je n'ai pas réussi à mettre à jour le dépôt git contenant les tests." >&2
  if [ -n "$(git status --porcelain --untracked-files=no 2> /dev/null)" ]; then
    printf " Si vous avez modifié le contenu du dossier de tests, je ne pourrai pas mettre à \
jour celui-ci tant que vous n'aurez pas annulé vos modifications (avec git restore).\n" >&2
  else
    printf "\n"
  fi
fi

exec bash ./autotests-stage-2.sh "$@"
