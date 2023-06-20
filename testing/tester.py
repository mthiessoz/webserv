import unittest
import os
import requests

get_endpoint = "/42lWatch.html"
get_cgi = "/cgi_script.py"
sleep_cgi = "/sleep.py"
post_endpoint = "/post.py"
bouh_txt = "/Users/esanchez/Projects/webserv/www/upload/bouh.txt"

class TestWebserver(unittest.TestCase):
    BASE_URL = 'http://localhost:8080'
    BASE_URL1 = 'http://localhost:8181'

    def test_get(self):
        response = requests.get(f'{self.BASE_URL}')
        self.assertEqual(response.status_code, 200)

    def test_404(self):
        response = requests.get(f'{self.BASE_URL}/notfoubddd')
        self.assertEqual(response.status_code, 404)

    def test_get2(self):    
        response = requests.get(f'{self.BASE_URL}{get_endpoint}')
        self.assertEqual(response.status_code, 200)

    def test_get3(self):    
        response = requests.get(f'{self.BASE_URL}{get_cgi}')
        self.assertEqual(response.status_code, 200)

    def test_get3(self):    
        response = requests.get(f'{self.BASE_URL}{sleep_cgi}')
        self.assertEqual(response.status_code, 508)

    def test_post_too_big(self):
        data = {"name": "eduardo"}
        response = requests.post(f'{self.BASE_URL}{post_endpoint}', json=data)
        self.assertEqual(response.status_code, 200)

    def test_post(self):
        data = {"name": "aaaaaaaaaaaaaaaaaaaaaaaaaaa"}
        response = requests.post(f'{self.BASE_URL}{post_endpoint}', json=data)
        self.assertEqual(response.status_code, 413)

    def test_delete(self):
        response = requests.delete(f'{self.BASE_URL}/upload/bouh.txt')
        self.assertEqual(response.status_code, 202)

    with open(os.path.join(os.getcwd(), bouh_txt), 'w') as file:
        file.write("Balalala/nbloubloub")

    def test_bad_delete(self):
        response = requests.delete(f'{self.BASE_URL}/upload/dontexist.txt')
        self.assertEqual(response.status_code, 404)


if __name__ == '__main__':
    unittest.main()
