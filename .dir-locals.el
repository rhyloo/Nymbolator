((c-mode . ((eval . (setq-local my/c-eldoc-includes
                             (list (concat (projectile-project-root) "include"))))))
 (c++-mode . ((eval . (setq-local my/c-eldoc-includes
                               (list (concat (projectile-project-root) "include")))))))
