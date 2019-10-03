--A medias
SELECT city.city_id, city.city from city, address, customer, rental
where city.city_id = address.city_id
and address.address_id = customer.address_id
and customer.customer_id = rental.customer_id



select rental_id FROM actor, film_actor,inventory,rental WHERE actor.first_name = 'Bob'
AND
actor.last_name = 'Fawcett'
AND
actor.actor_id = film_actor.actor_id
AND
film_actor.film_id = inventory.film_id
AND
inventory.inventory_id = rental.inventory_id;



--List the cities where movies, in which “Bob Fawcett” appears, have been rented.
--Each city must appear just once. Sort the output alphabetically by the city
--name from A to Z. The query must return the city id and the city.name.
