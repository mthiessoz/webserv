from locust import HttpUser, task, between

get_endpoint = "/42lWatch.html"
get_cgi = "/cgi_script.py"
post_endpoint = "/post.py"

class WebServerStressTest(HttpUser):
    wait_time = between(1, 2.5)

    @task
    def get_endpoint(self):
        self.client.get(get_endpoint)

    @task
    def cgi_endpoint(self):
        self.client.get(get_cgi)

    @task
    def post_endpoint(self):
        self.client.post(post_endpoint, json={"name": "eduardo"})