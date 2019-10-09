-- QUERY2

-- Laura de Paz and Paula Samper

-- Client who has rented more movies. If there is a tie between several clients,
-- all clients with the maximum number of rented movies must appear in the
-- response. The query must return the attributes: customer id, first name,
-- last name and the number of rented movies.


SELECT customer.customer_id,
       customer.first_name,
       customer.last_name,
       count(*) AS movies
FROM   rental,
       customer
WHERE  rental.customer_id = customer.customer_id --We relate the customer and the rental
GROUP  BY customer.customer_id -- We group and count the number of films by the customer_id
HAVING count(*) IN  (SELECT Count(*)
                    FROM   rental
                    GROUP  BY customer_id
                    ORDER  BY Count(*) DESC
                    LIMIT  1); -- This subquery counts the number of rentals per customer,
                               -- we order them in descending order and choose the first one
