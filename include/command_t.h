#include <stdint.h>

// ---------------------------
// Définition du type string
// ---------------------------
typedef struct {
    uint32_t length;  // longueur sans le '\0'
    char *data;       // pointeur vers les octets de la chaîne
} string_t;

// ---------------------------
// Définition du type arguments
// ---------------------------
typedef struct {
    uint32_t argc;
    string_t *argv;   // tableau de chaînes (de taille argc)
} arguments_t;

// ---------------------------
// Enumération du type de commande
// ---------------------------

typedef enum {SI,   // Commande simple
              SQ,   // Séquence
              PL,   // Pipeline
              IF_,  // Conditionnelle (le _ évite le conflit avec le mot-clé "if")
              WH,   // Boucle "while"
              UN    // Type inconnu / autre
} command_type_t;

// ---------------------------
// Définition du type command
// ---------------------------
typedef struct command {
    uint16_t type;            // 'SI' ou 'SQ' (valeur big-endian en sérialisation)
    arguments_t args;         // utilisé seulement si type == CMD_SI
    
    uint32_t nb_cmds;         // utilisé seulement si type != CMD_SI
    struct command *cmds;     // tableau de sous-commandes (taille nb_cmds)
} command_t;

