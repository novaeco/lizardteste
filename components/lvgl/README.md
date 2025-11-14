# LVGL (placeholder)

Ce répertoire est conservé pour compatibilité avec les scripts et
anciennes documentations qui s'attendaient à trouver les sources LVGL
vendues localement.

- **Sources LVGL** : elles ne sont plus présentes ici. Le projet dépend
  désormais de `lvgl/lvgl` (version 9.4.x) via le Component Manager
  d'ESP-IDF, déclaré dans `main/idf_component.yml`.
- **Configuration** : la configuration runtime reste disponible dans
  `components/lvgl_config/lv_conf.h`.
- **Build** : le fichier `CMakeLists.txt` se contente de faire `return()`
  afin de ne pas enregistrer de composant local nommé `lvgl`, évitant
  ainsi tout conflit avec le paquet géré.

Si vous avez encore une copie locale des sources LVGL, supprimez-les ou
excluez-les du dépôt pour éviter les diffs volumineux.
