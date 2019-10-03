-- QUERY2

SELECT customer.customer_id,
       customer.first_name,
       customer.last_name,
       count(*) AS movies
FROM   rental,
       customer
WHERE  rental.customer_id = customer.customer_id
GROUP  BY customer.customer_id
HAVING count(*) IN  (SELECT Count(*)
                    FROM   rental
                    GROUP  BY customer_id
                    ORDER  BY Count(*) DESC
                    LIMIT  1);
