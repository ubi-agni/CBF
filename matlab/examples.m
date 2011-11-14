# heftiger Sprung am Anfang
simple_2d_pinv_sim(0.05, [-2; 3], zeros(3,1), 0.1, 0.5)
# smooth clipping - mit geeignetem Threshold (0.5) - macht eine glatte Bewegung
simple_2d_svd_sim(0.05, [-2; 3], zeros(3,1), 0.1, 0.5)
# nur die task-Richtung aufzunehmen führt noch zu starker Oszillation
simple_2d_pinv_simple_virtual_task_weighted_sim(0.05, [-2; 3], zeros(3,1), 0.1, 1.0, 0.0)
# volle virtuelle Tasks-Matrix führt zu sehr langsamer Konvergenz am Ende
simple_2d_pinv_simple_virtual_task_weighted_sim(0.05, [-2; 3], zeros(3,1), 0.1, 0.0, 1.0)
# spielen mit der Gewichtung führt zur Lösung
simple_2d_pinv_simple_virtual_task_weighted_sim(0.05, [-2; 3], zeros(3,1), 0.1, 0.0, 0.5) 