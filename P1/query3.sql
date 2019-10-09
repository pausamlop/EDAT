-- QUERY3

-- Laura de Paz and Paula Samper

-- List the cities where movies, in which “Bob Fawcett” appears, have been rented.
-- Each city must appear just once. Sort the output alphabetically by the city
-- name from A to Z. The query must return the city id and the city.name.


SELECT  DISTINCT city, --We use distinct so we get the city just once at the end
       city.city_id
FROM   city,
       address,
       customer,
       rental,
       (SELECT rental_id
        FROM   actor,
               film_actor,
               inventory,
               rental
        WHERE  actor.first_name = 'Bob'
               AND actor.last_name = 'Fawcett'
               AND actor.actor_id = film_actor.actor_id
               AND film_actor.film_id = inventory.film_id
               AND inventory.inventory_id = rental.inventory_id) AS bf
               -- We create a subquery to relate the actor to the rentals of the films in
               -- which this actor has worked. We call it bf. 

WHERE  city.city_id = address.city_id
       AND address.address_id = customer.address_id
       AND customer.customer_id = rental.customer_id
       AND rental.rental_id = bf.rental_id -- We find the intersection between the rentals of a city and bs
ORDER  BY city;
--We order the resulting list by the city from A to Z by default
