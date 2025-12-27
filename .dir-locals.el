((c-mode . ((eval . (setq-local my/c-eldoc-includes
                             (list (concat (projectile-project-root) "include"))))))
 (c++-mode . ((eval . (setq-local my/c-eldoc-includes
                               (list (concat (projectile-project-root) "include")))))))

((nil . ((company-clang-arguments . ("-I/home/rhyloo/nymbolator/include/")))))

((c-mode . ((default-directory . "/home/rhyloo/Documents/nymbolator/")
            (irony-additional-clang-options . ("-I/home/rhyloo/Documents/nymbolator/include"))))
 (c++-mode . ((default-directory . "/home/rhyloo/Documents/nymbolator/")
              (irony-additional-clang-options . ("-I/home/rhyloo/Documents/nymbolator/include")))))
