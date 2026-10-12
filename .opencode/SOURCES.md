# Skills del proyecto — procedencia y licencias

Estas skills se han **copiado tal cual** (sin inventar ni reescribir su contenido) desde
repositorios públicos, y se han instalado en `.opencode/skills/` para que OpenCode las
descubra en este proyecto.

El objetivo es **aprender**: acompañar a quien programa durante el proyecto *Codexion*,
guiándole para que entienda y escriba él mismo el código, en lugar de generárselo.

## Skills instaladas

| ID (carpeta) | Fuente (copia literal) | Licencia | Para qué sirve aquí |
|---|---|---|---|
| `teaching-style` | Skill local ya existente en `~/.config/opencode/skills/teaching-style/SKILL.md` | propia del usuario | Estilo de enseñanza en español: paso a paso, preguntas guía, no dar el código, explicar el porqué. |
| `socratic-tutor` | https://gist.github.com/Shunseii/8cd4ef40718a2f2966ca8f30fa17cad6 | gist público, sin licencia explícita | Modo tutor para escribir código: pistas en vez de respuestas, revisa tu código sin reescribirlo. |
| `socrates` | https://github.com/bevibing/socrates-skill (`SKILL.md`) | MIT © 2026 RoundTable02 | Método socrático puro: nunca da la respuesta, te guía con preguntas. |
| `teach` | https://github.com/mattpocock/skills → `skills/productivity/teach/` | ver repo de origen | Espacio de enseñanza persistente: misión, lecciones, glosario y registros de aprendizaje entre sesiones. |
| `software-design-philosophy` | https://github.com/luoling8192/software-design-philosophy-skill (`SKILL.md`) | MIT | Diseño de software (Ousterhout): módulos profundos, ocultar información, nombrar bien, reducir complejidad. |

### Ficheros de apoyo de `teach`

`SKILL.md` referencia cuatro plantillas, copiadas también literalmente en la misma carpeta:

- `MISSION-FORMAT.md`
- `RESOURCES-FORMAT.md`
- `GLOSSARY-FORMAT.md`
- `LEARNING-RECORD-FORMAT.md`

## Notas de fidelidad / portabilidad

- **`teach`**: conserva el frontmatter original `disable-model-invocation: true`
  (quien lo definió lo diseñó como comando explícito). En OpenCode esto significa que
  **no se ofrece automáticamente al modelo**; se carga invocándolo a mano con `@teach`.
  Si se quisiera que el agente pueda activarlo solo, bastaría con quitar esa línea —
  no se ha hecho para respetar el original.
- **`socratic-tutor`**: el texto original menciona ejemplos en TypeScript/React; son
  ejemplos ilustrativos, no instrucciones para este proyecto en C.
- **Sin skills inventadas**: no existe skill pública (descargable) específica de
  Norminette/42 ni de pthreads en C, así que **no se ha fabricado ninguna**. Si se
  quiere una guía de la Norm, lo correcto es usar el PDF oficial:
  https://github.com/42School/norminette/blob/master/pdf/en.norm.pdf

## Cómo se usan en OpenCode

- El agente las descubre por su `description` y las carga cuando encajan con la petición.
- También se pueden forzar manualmente mencionándolas en el prompt: `@socrates`,
  `@socratic-tutor`, `@teaching-style`, `@software-design-philosophy`, `@teach`.

## Ubicación

```
.opencode/skills/
├── socrates/SKILL.md
├── socratic-tutor/SKILL.md
├── software-design-philosophy/SKILL.md
├── teach/
│   ├── SKILL.md
│   ├── GLOSSARY-FORMAT.md
│   ├── LEARNING-RECORD-FORMAT.md
│   ├── MISSION-FORMAT.md
│   └── RESOURCES-FORMAT.md
└── teaching-style/SKILL.md
```
