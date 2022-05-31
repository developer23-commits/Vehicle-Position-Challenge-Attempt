# Vehicle-Position-Challenge-Attempt

## Problem solving approach
 * Attempted to sort the vehicle positions by either longitude or latitude unsuccessful
   * The amount of vehicle positions in the file is way to high for this approach
   
 * Create a search radius to eliminate points that fall outside of the radius successful
	* Reduces the number of points to loop through
	* Use one of the 10 vehicle positions as the center point of the search radius
	* Select the vehicle position that is the closest to the "center" of the 10 vehicle points for the lowest radius
	
* Reduce the search radius for each of the 10 points unsuccessful
	* Reduce the number of positions even more
	* Sort the distances between the center point and each of the 10 vehicle positions from high to low
	* Assign the distances to the search radius. Ending up with 10 different search radius 
	* Reduce number of vehicle positions to loop through with every reduction in search radius
	* The copy of vehicle points into a filtered/reduced buffer cost to much time therefore this approach was unsuccessful
	

