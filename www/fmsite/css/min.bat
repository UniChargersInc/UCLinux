REM m_load.js = 9185

::perl -i.bk -pe 's/[^[:ascii:]]//g;' m_load.js

::sed -i "s/[\d128-\d255]//g"  m_load.js

::perl -pi -e 's/[^[:ascii:]]//g' m_load.js

::minify site.css style.css grid.css > site_style_grid.min.css

minify site.css grid.css style.css > site_style_grid.min.css