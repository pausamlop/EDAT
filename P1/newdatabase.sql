--
-- Name: former_employee; Type: TABLE; Schema: public; Owner: alumnodb
--

CREATE TABLE public.former_employee (
    staff_id smallint  NOT NULL,
    store_id smallint NOT NULL,
    to_date date NOT NULL,
    from_date date NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);



ALTER TABLE public.former_employee OWNER TO alumnodb;


--
-- Name: former_employee former_employee_pkey; Type: CONSTRAINT; Schema: public; Owner: alumnodb
--

ALTER TABLE ONLY public.former_employee
    ADD CONSTRAINT former_employee_pkey PRIMARY KEY (staff_id, store_id);


--
-- Name: former_employee former_employee_staff_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: alumnodb
--

ALTER TABLE ONLY public.former_employee
    ADD CONSTRAINT former_employee_staff_id_fkey FOREIGN KEY (staff_id) REFERENCES public.staff(staff_id) ON UPDATE CASCADE ON DELETE RESTRICT;


--
-- Name: former_employee former_employee_store_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: alumnodb
--

ALTER TABLE ONLY public.former_employee
    ADD CONSTRAINT former_employee_store_id_fkey FOREIGN KEY (store_id) REFERENCES public.store(store_id) ON UPDATE CASCADE ON DELETE RESTRICT;



--
-- Name: staff; Type: ADD COLUMN; Schema: public; Owner: alumnodb
--

ALTER TABLE public.staff
  ADD COLUMN date_from date;
