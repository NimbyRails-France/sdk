FROM python:3.12-alpine

# Only reviewed, image-owned sources may run with PLUGIN_TOKEN.
COPY image/ /opt/nrf/
RUN python3 -I /opt/nrf/test_publisher.py

ENTRYPOINT ["python3", "-I", "/opt/nrf/publish.py"]
