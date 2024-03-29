-- QUERY6

-- Laura de Paz and Paula Samper

-- Favorite category (category.name) of the customer who has rented more movies. By favorite category we refer
-- to the category in which the client has made more rentals. If a client rents the same movie twice
-- it should count as two rentals. The query must return the name (category.name) and the identifier
-- (category id) of the category. If there is a tie between several clients, all clients who have
-- rented more films should appear in the response.

-- We create a view so we can avoid creating several subqueries

CREATE view top_rent_cat
AS
  (SELECT rental.customer_id,
          category.name,
          Count(*) AS total
   FROM   rental,
          inventory,
          film,
          film_category,
          category,
          (SELECT rental.customer_id AS top,
                  Count(*)           AS num
           FROM   rental
           GROUP  BY rental.customer_id
           ORDER  BY num DESC
           LIMIT  1) AS top_rent
           -- We find the customer that has made more rentals by counting the times that the
           -- same rental.customer_id appears and choosing the one that appears the most (top_rent)
   WHERE  rental.inventory_id = inventory.inventory_id
          AND inventory.film_id = film.film_id
          AND film.film_id = film_category.film_id
          AND film_category.category_id = category.category_id
          AND rental.customer_id = top_rent.top
   GROUP  BY rental.customer_id,
             category.name);
             --We relate the rental to the category and the rental.customer to top_rent

SELECT name
FROM   top_rent_cat
ORDER  BY total DESC
LIMIT  1;
--We select the category with the highest amount of rentals(of the top_rent)

DROP VIEW top_rent_cat;
