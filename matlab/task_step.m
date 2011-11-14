function step = task_step (delta, current_task, target_task)
  step = target_task - current_task;
  size = norm(step);
  if (size > delta)
	 step *= delta / size;
  endif
end