#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "serialization.h"
#include "task_tree.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// OPCODES - Codes d'opération pour les requêtes client -> démon
// ============================================================================
#define OPCODE_LIST           0x4c53  // 'LS' - Lister toutes les tâches
#define OPCODE_CREATE         0x4352  // 'CR' - Créer une nouvelle tâche simple
#define OPCODE_COMBINE        0x4342  // 'CB' - Créer une tâche par combinaison
#define OPCODE_REMOVE         0x524d  // 'RM' - Supprimer une tâche
#define OPCODE_TIMES_EXITCODES 0x5458  // 'TX' - Historique d'exécution
#define OPCODE_STDOUT        0x534f  // 'SO' - Sortie standard
#define OPCODE_STDERR        0x5345  // 'SE' - Sortie erreur
#define OPCODE_TERMINATE     0x544d  // 'TM' - Terminer le démon

// ============================================================================
// ANSTYPE - Types de réponse démon -> client
// ============================================================================
#define ANSTYPE_OK           0x4f4b  // 'OK' - Requête réussie
#define ANSTYPE_ERROR        0x4552  // 'ER' - Requête échouée

// ============================================================================
// ERRCODE - Codes d'erreur pour les réponses ERROR
// ============================================================================
#define ERRCODE_NOT_FOUND    0x4e46  // 'NF' - Tâche non trouvée
#define ERRCODE_NOT_RUN      0x4e52  // 'NR' - Tâche jamais exécutée

// ============================================================================
// Structure pour les requêtes (client -> démon)
// ============================================================================
typedef struct {
    uint16_t opcode;  // Code d'opération (OPCODE_*)
    union {
        // Pour OPCODE_CREATE
        struct {
            timing_t timing;
            uint32_t argc;     // Nombre d'arguments
            char **argv;       // Tableau des arguments
        } create;
        
        // Pour OPCODE_COMBINE
        struct {
            timing_t timing;
            uint16_t type;        // Type de combinaison (CMD_TYPE_SIMPLE ou CMD_TYPE_SEQUENCE)
            uint32_t nbtasks;     // Nombre de tâches à combiner
            uint64_t *taskids;    // Tableau des identifiants de tâches
        } combine;
        
        // Pour OPCODE_REMOVE, OPCODE_TIMES_EXITCODES, OPCODE_STDOUT, OPCODE_STDERR
        struct {
            uint64_t taskid;
        } query;
        
        // Pour OPCODE_LIST et OPCODE_TERMINATE : pas de données supplémentaires
    } u;
} request_t;

// ============================================================================
// Structure pour les réponses (démon -> client)
// ============================================================================
typedef struct {
    uint16_t anstype;  // Type de réponse (ANSTYPE_OK ou ANSTYPE_ERROR)
    union {
        // Réponse OK pour CREATE ou COMBINE
        struct {
            uint64_t taskid;  // Identifiant de la tâche créée
        } create_ok;
        
        // Réponse OK pour LIST
        struct {
            uint32_t nbtasks;  // Nombre de tâches
            task_t **tasks;    // Tableau de pointeurs vers les tâches
        } list_ok;
        
        // Réponse OK pour TIMES_EXITCODES
        struct {
            uint32_t nbruns;        // Nombre d'exécutions
            int64_t *timestamps;    // Tableau des timestamps (secondes depuis epoch)
            uint16_t *exitcodes;    // Tableau des codes de retour
        } times_exitcodes_ok;
        
        // Réponse OK pour STDOUT ou STDERR
        struct {
            char *output;  // Contenu de la sortie (alloué dynamiquement)
            size_t len;    // Longueur en octets (sans le '\0' final)
        } output_ok;
        
        // Réponse ERROR
        struct {
            uint16_t errcode;  // Code d'erreur (ERRCODE_*)
        } error;
        
        // Réponse OK pour REMOVE ou TERMINATE : pas de données supplémentaires
    } u;
} response_t;

// ============================================================================
// Prototypes des fonctions de gestion des FIFO
// ============================================================================

/**
 * Initialise les tubes nommés (FIFO) pour la communication.
 * Crée les FIFO request et reply dans le répertoire run_dir s'ils n'existent pas.
 *
 * @param run_dir Répertoire de base (ex: /tmp/$USER/erraid)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int init_pipes(const char *run_dir);

/**
 * Ouvre les tubes nommés pour le démon.
 * Le démon lit les requêtes et écrit les réponses.
 *
 * @param run_dir Répertoire de base
 * @param request_fd_out Pointeur qui recevra le descripteur de lecture des requêtes
 * @param reply_fd_out   Pointeur qui recevra le descripteur d'écriture des réponses
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int open_pipes_daemon(const char *run_dir, int *request_fd_out, int *reply_fd_out);

/**
 * Ouvre les tubes nommés pour le client.
 * Le client écrit les requêtes et lit les réponses.
 *
 * @param run_dir Répertoire de base
 * @param request_fd_out Pointeur qui recevra le descripteur d'écriture des requêtes
 * @param reply_fd_out   Pointeur qui recevra le descripteur de lecture des réponses
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int open_pipes_client(const char *run_dir, int *request_fd_out, int *reply_fd_out);

// ============================================================================
// Prototypes des fonctions de sérialisation/désérialisation
// ============================================================================

/**
 * Envoie une requête sur un descripteur de fichier (FIFO).
 * 
 * @param fd   Descripteur de fichier ouvert en écriture
 * @param req  Requête à envoyer (non NULL)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int send_request(int fd, const request_t *req);

/**
 * Reçoit une requête depuis un descripteur de fichier (FIFO).
 * 
 * @param fd   Descripteur de fichier ouvert en lecture
 * @param req  Pointeur vers un pointeur qui recevra la requête allouée (non NULL)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 * 
 * @note La requête doit être libérée avec free_request() après utilisation.
 */
int receive_request(int fd, request_t **req);

/**
 * Envoie une réponse sur un descripteur de fichier (FIFO).
 * 
 * @param fd   Descripteur de fichier ouvert en écriture
 * @param resp Réponse à envoyer (non NULL)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int send_response(int fd, const response_t *resp);

/**
 * Reçoit une réponse depuis un descripteur de fichier (FIFO).
 * 
 * @param fd   Descripteur de fichier ouvert en lecture
 * @param resp Pointeur vers un pointeur qui recevra la réponse allouée (non NULL)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 * 
 * @note La réponse doit être libérée avec free_response() après utilisation.
 */
int receive_response(int fd, response_t **resp);

// ============================================================================
// Prototypes des fonctions de libération de mémoire
// ============================================================================

/**
 * Libère la mémoire allouée pour une requête.
 * 
 * @param req Requête à libérer (peut être NULL)
 */
void free_request(request_t *req);

/**
 * Libère la mémoire allouée pour une réponse.
 * 
 * @param resp Réponse à libérer (peut être NULL)
 */
void free_response(response_t *resp);

#endif // PROTOCOL_H
