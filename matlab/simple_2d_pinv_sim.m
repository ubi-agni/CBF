
function simple_2d_pinv_sim(delta, target_task, start_phi, residual)
	current_phi = start_phi;
	plot_robot(current_phi);
	current_task = fk(start_phi);
	while(norm(current_task - target_task) > residual)
		delta_task = task_step(delta, current_task, target_task);
		delta_phi = pinv(jac(current_phi)) * delta_task;
		current_phi += delta_phi;
		
		plot_robot(current_phi);
		sleep(0.01)
		
		current_task = fk(current_phi);
	end
	
end