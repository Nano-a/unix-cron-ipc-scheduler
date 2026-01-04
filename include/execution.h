#ifndef EXECUTION_H
#define EXECUTION_H

#include <stddef.h>
#include <stdint.h>

#include "serialization.h"

/**
 * Exécute une commande simple (cmd->argv) et capture stdout/stderr.
 *
 * @param cmd          Commande à exécuter (doit avoir argc > 0, nb_cmds == 0)
 * @param stdout_buf   Retourne un tampon alloué contenant stdout (NULL si vide)
 * @param stdout_len   Taille utile de stdout (sans le '\0' final)
 * @param stderr_buf   Retourne un tampon alloué contenant stderr (NULL si vide)
 * @param stderr_len   Taille utile de stderr (sans le '\0' final)
 * @param exitcode     Code de retour du processus fils
 *
 * @return 0 si succès, -1 sinon (errno positionnée)
 */
int execute_simple_command(const command_t *cmd,
                           char **stdout_buf, size_t *stdout_len,
                           char **stderr_buf, size_t *stderr_len,
                           uint16_t *exitcode);

int execute_sequence_command(const command_t *cmd,
                             char **stdout_buf, size_t *stdout_len,
                             char **stderr_buf, size_t *stderr_len,
                             uint16_t *exitcode);

int execute_pipeline_command(const command_t *cmd,
                              char **stdout_buf, size_t *stdout_len,
                              char **stderr_buf, size_t *stderr_len,
                              uint16_t *exitcode);

int execute_conditional_command(const command_t *cmd,
                                char **stdout_buf, size_t *stdout_len,
                                char **stderr_buf, size_t *stderr_len,
                                uint16_t *exitcode);

#endif // EXECUTION_H

