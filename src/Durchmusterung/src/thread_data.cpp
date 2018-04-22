#include "thread_data.h"

/*! \brief constructor for the thread_data object
 *
 *
 *  The constructor requires a valid tile_worker object where it stores the pointer to. Only read access is needed.
 */
thread_data::thread_data(tile_worker* tile_worker) {
	my_tile_worker = tile_worker;
}

/*! \brief pointer to function which is given to p_thread.
 *
 *
 *  Within this function the p_thread is doing its work. First a new starting point
 *  to scan is requested by the master. If no more available, the thread is terminated.
 *  If a valid starting point is available then the thread is fetching point after point
 *  which is located in the requested plane orientation. For each point, several
 *  checks are performed e.g. slope with previous point, slope with orthogonal
 *  points, validity of data point value itself, .... if all criteria match the
 *  job parameter, then this point is added to a vector (list). This procedure
 *  is repeated until no further points are left of the current point does not
 *  fulfill the requested parameters. Then the current collected points in the
 *  vector (list) are checked whether they fulfill the min. length of the plane.
 *  If this is the case a subroutine is called for calculating the longest plane
 *  and the plane with min variance but fulfilling the min. length criteria.
 @retval NULL thread is done
 */
void* thread_data::check_single_plane(
		void* x_void_ptr /** [in,out] pointer to the tile_worker object */) {
	tile_worker* my_tile_worker = ((thread_data*) x_void_ptr)->my_tile_worker;
	int completed = 0;
	int current_in_a_row = 0;
	double i;
	double j;
	int checksum = 0;
	int previous_x = 0;
	int previous_y = 0;
	int previous_valid = 0;

	while (!my_tile_worker->get_start_values(i, j)) // checks whether starting points for scanning are left
	{
		vector<pair<int, int> > coordlist;

		while (!completed) {
			++checksum;

			if (my_tile_worker->not_defined == NULL
					|| *my_tile_worker->not_defined
							!= my_tile_worker->access_single_element(i, j)) // checks whether current point
				                                                            //has valid geo information
									{
				if (previous_valid) // checks whether it is not the first point in a row
				{
					if (fabs(
							my_tile_worker->access_single_element(i, j)
									- my_tile_worker->access_single_element(
											previous_x, previous_y))
							< my_tile_worker->max_diff_neighbours /*longitudinal*/) // check diff between
						                                    //neighboured points in plane direction
							{
                                                int ok = 1;
                                                // now check for short range slope
                                                double fx = 1;
                                                double fy = 1;
                                                if (previous_x < i)
                                                  fx=1;
                                                else
                                                  fx=-1;
                                                if (previous_y < j)
                                                  fy=1;
                                                else
                                                  fy=-1;
                                                if ((abs (i-coordlist[0].first) > my_tile_worker->diff_short_x_longitudinal) && (abs(j-coordlist[0].second) > my_tile_worker->diff_short_y_longitudinal))
                                                {
                                                float myprevpoint = my_tile_worker->access_single_element((i+fx*my_tile_worker->diff_short_x_longitudinal), (j+fy*my_tile_worker->diff_short_y_longitudinal));

                                                if (myprevpoint != numeric_limits<float>::min())
                                                {
                                                    if (fabs(
                                                        my_tile_worker->access_single_element(i, j)
                                                                - myprevpoint)
                                                        > my_tile_worker->allowed_diff_short_range_slope /*longitudinal*/)
                                                          ok =0;
                                                }
                                                }
                                                 // now loop over all orthogonal elements
						for (int k = 0; k < 2; k++) // loop in two directions
								{
							double new_x = i;
							double new_y = j;
							double factor = pow(-1, k);

							for (int l = 0;
									l
											< my_tile_worker->needed_orthogonal_points_in_a_row;
									l++) // loop over all needed orthogonal points
									{
								double old_x = new_x;
								double old_y = new_y;
								new_x += factor * my_tile_worker->orth_x;
								new_y += factor * my_tile_worker->orth_y;

								if (((new_x >= 0)
										&& (new_x
												< my_tile_worker->tile->width.x))
										&& ((new_y >= 0)
												&& (new_y
														< my_tile_worker->tile->width.y))) // vaild range check
										{
									if (fabs(
											my_tile_worker->access_single_element(
													new_x, new_y)
													- my_tile_worker->access_single_element(
															old_x, old_y))
											> my_tile_worker->max_diff_neighbours /*transversal*/) // check for orthogonal diff
											{
										ok = 0;
									}

									if (fabs(
											my_tile_worker->access_single_element(
													new_x, new_y)
													- my_tile_worker->access_single_element(
															new_x
																	- my_tile_worker->inc_x,
															new_y
																	- my_tile_worker->inc_y))
											> my_tile_worker->max_diff_neighbours /*longitudinal*/) // check for diff in plane direction for all orthogonal neighbours
											{
										ok = 0;
									}
								} else {
									ok = 0;
								}
							}
						}

						if (ok) {
							++current_in_a_row;
							coordlist.push_back(make_pair(i, j)); // add current point
						}

						if (!ok) // current point is not valid but it might be that current coordlist already has enough valid points
						{
							my_tile_worker->check_current_landebahn(
									current_in_a_row, coordlist);
						}
					} else // current point is not valid but it might be that current coordlist already has enough valid points
					{
						my_tile_worker->check_current_landebahn(
								current_in_a_row, coordlist);
					}
				}

				previous_valid = 1;
			} else {
				//current point not def
				my_tile_worker->check_current_landebahn(current_in_a_row,
						coordlist);
			}

			// move point forward
			previous_x = i;
			previous_y = j;
			i += my_tile_worker->inc_x;
			j += my_tile_worker->inc_y;

			// now we have to check whether the new point is still in the range of valid tile. If not collected points have to be checked to have a potential valid plane
			if (my_tile_worker->direction == 1) {
				if (j >= my_tile_worker->tile->width.y) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 2) {
				if ((i < 0) || (j >= my_tile_worker->tile->width.y)) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 3) {
				if (i < 0) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 4) {
				if ((i < 0) || (j < 0)) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 5) {
				if (j < 0) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 6) {
				if ((i > my_tile_worker->tile->width.x - 1) || (j < 0)) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 7) {
				if (i > my_tile_worker->tile->width.x - 1) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}

			if (my_tile_worker->direction == 8) {
				if ((i > my_tile_worker->tile->width.x - 1)
						|| (j > my_tile_worker->tile->width.y - 1)) {
					my_tile_worker->check_current_landebahn(current_in_a_row,
							coordlist);
					break;
				}
			}
		}
	}

	// here the outer loop quits which indicates that no starting points are left
	sem_post(my_tile_worker->count_sem);
	/* the function must return something - NULL will do */
	return NULL;
}
