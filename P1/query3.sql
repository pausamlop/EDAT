-- QUERY3
-- List the cities where movies, in which “Bob Fawcett” appears, have been rented.
-- Each city must appear just once. Sort the output alphabetically by the city
-- name from A to Z. The query must return the city id and the city.name.


SELECT city,
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

WHERE  city.city_id = address.city_id
       AND address.address_id = customer.address_id
       AND customer.customer_id = rental.customer_id
       AND rental.rental_id = bf.rental_id
ORDER  BY city;
