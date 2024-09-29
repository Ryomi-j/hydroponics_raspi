import path from "path";

const nextConfig = {
  webpack(config) {
    config.module.rules.push({
      test: /\.svg$/,
      include: path.resolve("./public/assets"),
      use: [
        {
          loader: "@svgr/webpack",
          options: {
            svgo: true,
          },
        },
      ],
    });

    return config;
  },
};

export default nextConfig;
